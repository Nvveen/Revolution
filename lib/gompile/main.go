// This file is part of Revolution.
// 
// Revolution is free software: you can redistribute it and/or modify it under 
// the terms of the GNU General Public License as published by the 
// Free Software Foundation, either version 3 of the License, or 
// (at your option) any later version.
// 
// Revolution is distributed in the hope that it will be useful, but WITHOUT 
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
// more details.
// 
// You should have received a copy of the GNU General Public License along with 
// Revolution. If not, see <http://www.gnu.org/licenses/>.

// Gompile takes a zipped .kmz-file, reads in the data, triangulates it with
// Triangle and then compiles it into a binary file.
package main

import (
  "bufio"
  "bytes"
  "io"
  "archive/zip"
  "os"
  "os/exec"
  "fmt"
  "io/ioutil"
  "path"
  "encoding/xml"
  "regexp"
  "sync"
  "math/rand"
  "math"
)

var (
  pwd, tmpDir string
  writingMutex sync.Mutex
)

type (
  // XML Struct
  xmlKML struct {
    XMLName xml.Name `xml:"kml"`
    Document struct {
      Folders []struct {
        Placemarks []struct {
          Name string `xml:"name"`
          MultiGeometry []xmlPolygon `xml:"MultiGeometry>Polygon"`
          Polygon xmlPolygon `xml:"Polygon"`
        } `xml:"Placemark"`
      } `xml:"Folder"`
    }
  }
  xmlPolygon struct {
    OuterBoundaryIs xmlBoundaryIs `xml:"outerBoundaryIs"`
    InnerBoundaryIs xmlBoundaryIs `xml:"innerBoundaryIs"`
  }
  xmlBoundaryIs struct {
    LinearRing struct {
      Coordinates string `xml:"coordinates"`
    }
  }

  Country struct {
    Name string
    Regions []Region
    Path string
  }
  Region struct {
    OuterVertices, InnerVertices []Vertex
  }
  Vertex struct {
    X, Y float64
  }
)

func OpenZip(fn string) (b []byte) {
  // Unzip the file.
  r, err := zip.OpenReader(fn)
  if err != nil {
    panic(err)
  }
  defer r.Close()

  // Only one file may be in the kmz file
  if len(r.File) != 1 {
    panic(fmt.Errorf("Wrong number of KML files in %s\n", fn))
  }
  f := r.File[0]
  fmt.Printf("Deflating %s\n", f.Name)
  rc, err := f.Open()
  if err != nil {
    panic(err)
  }
  // Read entire file.
  b, err = ioutil.ReadAll(rc)
  if err != nil {
    panic(err)
  }
  rc.Close()
  return b
}

func NumCountries(kml *xmlKML) (int) {
  nr := 0
  for i, _ := range kml.Document.Folders {
    for _, _ = range kml.Document.Folders[i].Placemarks {
      nr++
    }
  }
  return nr
}

func ReadKML(kml *xmlKML, countries chan Country) {
  var wg sync.WaitGroup
  for i, folder := range kml.Document.Folders {
    for j, _ := range folder.Placemarks {
      wg.Add(1)
      // Iterate over every place and, in a seperate goroutine, read
      // all vertex data, pass it to the right channels and exit.
      go func(p, q int) {
        place := kml.Document.Folders[p].Placemarks[q]
        country := Country{Name: place.Name}
        fmt.Printf("Reading %s\n", country.Name)
        // Unify polygon interfacing
        polygons := &place.MultiGeometry
        if len(place.MultiGeometry) == 0 {
          polygons = &[]xmlPolygon{place.Polygon}
        }
        country.Regions = make([]Region, len(*polygons))
        // Extract inner and outer vertices from the string data representing
        // cleartext coordinates gathered from the XML data.
        for r, _ := range *polygons {
          outer, inner := (*polygons)[r].ExtractVertices()
          country.Regions[r].OuterVertices = outer
          country.Regions[r].InnerVertices = inner
        }
        countries <- country
        wg.Done()
      }(i, j)
    }
  }
  wg.Wait()
  return
}

func (p *xmlPolygon) ExtractVertices() ([]Vertex, []Vertex) {
  reg, _ := regexp.Compile(`-??[\d]+(\.[\d]+)?,-??[\d]+\.([\d]+)?`)
  // Functionality for inner and outer boundaries is the same, so just
  // use a function.
  f := func(boundary xmlBoundaryIs) []Vertex {
    vertData := boundary.LinearRing.Coordinates
    // Find all matches.
    matches := reg.FindAllString(vertData, -1)
    // Scan each match into a vertex, and add it to the list.
    verts := make([]Vertex, len(matches))
    for i, match := range matches {
      fmt.Sscanf(match, "%f,%f", &verts[i].X, &verts[i].Y)
    }
    return verts
  }
  return f(p.OuterBoundaryIs), f(p.InnerBoundaryIs)
}

func (c *Country) WritePoly () {
  c.Path = path.Join(tmpDir, c.Name)
  fmt.Printf("Writing %s\n", c.Path)
  os.Mkdir(c.Path, 0755)
  for i, region := range c.Regions {
    polyFile, err := os.Create(path.Join(c.Path,fmt.Sprintf("poly-%d.poly", i)))
    if err != nil {
      panic(err)
    }

    size := len(region.OuterVertices) + len(region.InnerVertices)
    polyFile.WriteString(fmt.Sprintf("%d 2 0 0\n", size))
    writeVertices := func(vertices []Vertex, offset int) {
      for j, vertex := range vertices {
        polyFile.WriteString(fmt.Sprintf("%d %f %f\n", j+offset, vertex.X,
                                         vertex.Y))
      }
    }
    writeEdges := func(vertices []Vertex, offset int) {
      for j, _ := range vertices {
        endpoint := j+offset+1
        if endpoint == len(vertices)+offset {
          endpoint = offset
        }
        polyFile.WriteString(fmt.Sprintf("%d %d %d 0\n", j+offset, j+offset,
                                         endpoint))
      }
    }
    writeVertices(region.OuterVertices, 0)
    writeVertices(region.InnerVertices, len(region.OuterVertices))
    polyFile.WriteString(fmt.Sprintf("%d\n", size))
    writeEdges(region.OuterVertices, 0)
    writeEdges(region.InnerVertices, len(region.OuterVertices))

    if len(region.InnerVertices) > 0 {
      polyFile.WriteString("1\n")
      v := DetermineHole(region.InnerVertices)
      polyFile.WriteString(fmt.Sprintf("0 %f %f\n", v.X, v.Y))
    } else {
      polyFile.WriteString("0\n")
    }

    polyFile.Close()
  }
}

func DetermineHole(vertices []Vertex) (Vertex) {
  min := vertices[0]
  max := vertices[0]
  for _, v := range vertices {
    if v.X < min.X {
      min.X = v.X
    }
    if v.Y < min.Y {
      min.Y = v.Y
    }
    if v.X > max.X {
      max.X = v.X
    }
    if v.Y > max.Y {
      max.Y = v.Y
    }
  }
  var x, y float64
  for {
    x, y = rand.Float64(), rand.Float64()
    x = (x * math.Abs(max.X-min.X)) + min.X
    y = (y * math.Abs(max.Y-min.Y)) + min.Y
    pip := false
    for i, j := 0, len(vertices)-1; i < len(vertices); j, i = i, i+1 {
      if ((vertices[i].Y > y) != (vertices[j].Y > y) &&
          (x < (vertices[j].X-vertices[i].X) * (y - vertices[i].Y /
          (vertices[j].Y - vertices[i].Y) + vertices[i].X))) {
        pip = !pip
      }
    }
    if pip {
      break
    }
  }
  return Vertex{x, y}
}

func (c *Country) FindSuperset() {
  // Simple:
  if len(c.Regions) > 1 {
    max := 0
    mark := -1
    for i, _ := range c.Regions {
      if max < len(c.Regions[i].OuterVertices) {
        max = len(c.Regions[i].OuterVertices)
        mark = i
      }
    }
    if mark > -1 {
      c.Regions = append(c.Regions[:mark], c.Regions[mark+1:]...)
    }
  }
}

func (c *Country) Triangulate() {
  fmt.Printf("Triangulating %s\n", c.Name)
  for i, _ := range c.Regions {
    filePath := path.Join(c.Path, fmt.Sprintf("poly-%d.poly", i))
    command := exec.Command(path.Join(pwd, "triangle"), filePath)
    output, err := command.CombinedOutput()
    if err != nil {
      panic(fmt.Errorf("%s: %s", err, string(output)))
    }
  }
}

func (c *Country) ReadTriangulated() {
  reg, _ := regexp.Compile(`[\d]+(.[\d]+)?`)
  for i, _ := range c.Regions {
    filePath := path.Join(c.Path, fmt.Sprintf("poly-%d.1", i))
    nodeFile, err := os.Open(filePath+".node")
    if err != nil {
      panic(err)
    }
    defer nodeFile.Close()
    var n int
    buf := bufio.NewReader(nodeFile)
    line, err := buf.ReadString('\n')
    if err != nil {
      panic(fmt.Errorf("Unexpected end of file"))
    }
    fmt.Sscanf(line, "%d", &n)
    c.Regions[i].OuterVertices = make([]Vertex, n)
    c.Regions[i].InnerVertices = nil
    for err != io.EOF {
      line, err = buf.ReadString('\n')
      if err != nil && err != io.EOF {
        panic(err)
      }
      matches := reg.FindAllString(line, -1)
      if len(matches) == 4 {
        vertex := Vertex{}
        index := -1
        fmt.Sscanf(matches[0], "%d", &index)
        fmt.Sscanf(matches[0], "%f", &vertex.X)
        fmt.Sscanf(matches[1], "%f", &vertex.Y)
        c.Regions[i].OuterVertices[index] = vertex
      }
    }
  }
}

func (c *Country) WriteBytes(rw io.ReadWriter) {
  writingMutex.Lock()
  writingMutex.Unlock()
}

func main () {
  defer func() {
    if r := recover(); r != nil {
      fmt.Fprint(os.Stderr, r)
      os.Exit(1)
    }
  }()
  // Determine argument count
  if len(os.Args) < 2 {
    panic(fmt.Errorf("Not enough arguments to commandline\n"))
  }
  // Determine paths
  pwd, _ = os.Getwd()
  tmpDir, _ = ioutil.TempDir("", "gompile")
  os.Chdir(tmpDir)
  // Determine input path
  inputFile := path.Clean(path.Join(pwd, path.Base(os.Args[1])))

  // Unzip
  fmt.Printf("Unzipping %s\n", inputFile)
  b := OpenZip(inputFile)

  // Unmarshal
  fmt.Printf("Unmarshalling %s\n", inputFile)
  kml := new(xmlKML)
  err := xml.Unmarshal(b, kml)
  if err != nil {
    panic(err)
  }

  // Read data
  nr := NumCountries(kml)
  countries := make(chan Country, nr)
  var wg, wg2 sync.WaitGroup
  wg.Add(1)
  go func() {
    ReadKML(kml, countries)
    close(countries)
    wg.Done()
  }()
  buf := new(bytes.Buffer)
  for c := range countries {
    wg2.Add(1)
    go func(country Country) {
      defer func(c Country) {
        if r := recover(); r != nil {
          fmt.Fprintf(os.Stderr, "Error in %s: %s\n", c.Name, r)
        }
      }(c)
      // country.FindSuperset()
      country.WritePoly()
      country.Triangulate()
      country.ReadTriangulated()
      country.WriteBytes(buf)
      wg2.Done()
    }(c)
  }
  wg2.Wait()
  wg.Wait()
}
