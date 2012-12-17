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
  tmpDir, execPath, outputPath string
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

  // Country-struct
  Country struct {
    Name string
    Regions []Region
    Path string
  }
  Region struct {
    OuterVertices, InnerVertices VertexList
    Triangles TriangleList
  }
  Vertex struct {
    X, Y float64
  }
  Triangle struct {
    T1, T2, T3 int
  }

  TriangleList []Triangle
  VertexList []Vertex

  Matcher interface {
    Match(match []string, i int)
    CreateList(n int)
  }
)

// (Re)set the triangle list
func (t *TriangleList) CreateList(n int) {
  *t = make(TriangleList, n)
}

// (Re)set the vertex list
func (v *VertexList) CreateList(n int) {
  *v = make(VertexList, n)
}

// Match a string-list to scan it into the Triangle list
func (t *TriangleList) Match(matches []string, i int) {
  fmt.Sscanf(matches[1], "%d", &(*t)[i].T1)
  fmt.Sscanf(matches[2], "%d", &(*t)[i].T2)
  fmt.Sscanf(matches[3], "%d", &(*t)[i].T3)
}

// Match a string list to scan it into the Vertex list
func (v *VertexList) Match(matches []string, i int) {
  fmt.Sscanf(matches[1], "%f", &(*v)[i].X)
  fmt.Sscanf(matches[2], "%f", &(*v)[i].Y)
}

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

// NumCountries counts the number of countries.
func NumCountries(kml *xmlKML) (int) {
  nr := 0
  for i, _ := range kml.Document.Folders {
    for _, _ = range kml.Document.Folders[i].Placemarks {
      nr++
    }
  }
  return nr
}

// ReadKML reads from a struct into a channel of countries, so we can
// concurrently process each country.
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

// ExtractVertices takes a polygon, and matches the string data of the 
// coordinates and scans them into two VertexLists.
func (p *xmlPolygon) ExtractVertices() (VertexList, VertexList) {
  reg, _ := regexp.Compile(`-??[\d]+(\.[\d]+)?,-??[\d]+\.([\d]+)?`)
  // Functionality for inner and outer boundaries is the same, so just
  // use a function.
  f := func(boundary xmlBoundaryIs) VertexList {
    vertData := boundary.LinearRing.Coordinates
    // Find all matches.
    matches := reg.FindAllString(vertData, -1)
    // Scan each match into a vertex, and add it to the list.
    var verts VertexList
    verts.CreateList(len(matches))
    for i, match := range matches {
      fmt.Sscanf(match, "%f,%f", &verts[i].X, &verts[i].Y)
    }
    return verts
  }
  return f(p.OuterBoundaryIs), f(p.InnerBoundaryIs)
}

// WritePoly writes country data in the .poly format needed by Triangle.
func (c *Country) WritePoly () {
  // First create a temporary directory.
  c.Path = path.Join(tmpDir, c.Name)
  fmt.Printf("Writing %s\n", c.Path)
  os.Mkdir(c.Path, 0755)
  // For each region, write a file.
  for i, region := range c.Regions {
    polyFile, err := os.Create(path.Join(c.Path,fmt.Sprintf("poly-%d.poly", i)))
    if err != nil {
      panic(err)
    }

    // For the specification of a poly-file, see the Triangle website.
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
    // Vertices and edges (segments) are written to the same file.
    writeVertices(region.OuterVertices, 0)
    writeVertices(region.InnerVertices, len(region.OuterVertices))
    polyFile.WriteString(fmt.Sprintf("%d\n", size))
    writeEdges(region.OuterVertices, 0)
    writeEdges(region.InnerVertices, len(region.OuterVertices))

    // Find the holes.
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

// DetermineHole uses an inefficient random-point generation to find a point
// inside the inner vertices to determine as a hole, so Triangle skips
// this in triangulation.
func DetermineHole(vertices VertexList) (Vertex) {
  // For minimum efficiency, find a minimum spanning box of the polygon.
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
  // Keep generating random points until one has been found that is inside
  // the polygon.
  var x, y float64
  for {
    // Generate a random point.
    x, y = rand.Float64(), rand.Float64()
    x = (x * math.Abs(max.X-min.X)) + min.X
    y = (y * math.Abs(max.Y-min.Y)) + min.Y
    // Next is an algorithm that determines if the point is inside.
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

// FindSuperset is a function that removes the superset of all regions
// that is included in most countries that are divided into regions.
func (c *Country) FindSuperset() {
}

// Triangulate calls the Triangle program on the generated poly file.
func (c *Country) Triangulate() {
  fmt.Printf("Triangulating %s\n", c.Name)
  for i, _ := range c.Regions {
    filePath := path.Join(c.Path, fmt.Sprintf("poly-%d.poly", i))
    command := exec.Command(path.Join(execPath, "triangle"), filePath)
    output, err := command.CombinedOutput()
    if err != nil {
      panic(fmt.Errorf("%s: %s", err, string(output)))
    }
  }
}

// ReadTriangulated reads the triangulated poly/ele/node files and creates
// new vertex/triangle lists for the designated countries.
func (c *Country) ReadTriangulated() {
  reg, _ := regexp.Compile(`-??[\d]+(.[\d]+)?`)
  for i, _ := range c.Regions {
    // Determine the path to the current region files
    filePath := path.Join(c.Path, fmt.Sprintf("poly-%d.1", i))
    // Open nodes first
    nodeFile, err := os.Open(filePath+".node")
    if err != nil {
      panic(err)
    }
    defer nodeFile.Close()

    // Add data from a buffer reader, resetting untriangulated data already
    // there.
    buf := bufio.NewReader(nodeFile)
    ReadList(buf, &c.Regions[i].OuterVertices, reg)
    edgeFile, err := os.Open(filePath+".ele")
    if err != nil {
      panic(err)
    }
    defer edgeFile.Close()
    buf = bufio.NewReader(edgeFile)
    ReadList(buf, &c.Regions[i].Triangles, reg)
  }
}

// ReadList takes a Matcher list, that is then read and scanned for data.
func ReadList(buf *bufio.Reader, mt Matcher, reg *regexp.Regexp) {
  line, err := buf.ReadString('\n')
  if err != nil {
    panic(fmt.Errorf("Unexpected end of file"))
  }
  var n int
  fmt.Sscanf(line, "%d", &n)
  // Reset the list.
  mt.CreateList(n)
  // Matches all lines and scan in.
  for err != io.EOF {
    line, err = buf.ReadString('\n')
    if err != nil && err != io.EOF {
      panic(err)
    }
    matches := reg.FindAllString(line, -1)
    if len(matches) == 4 {
      index := -1
      fmt.Sscanf(matches[0], "%d", &index)
      mt.Match(matches, index)
    }
  }
}

func (c *Country) WriteBack() {
  countryPath := path.Join(outputPath, c.Name)
  err := os.Mkdir(countryPath, 0755)
  if err != nil {
    panic(err)
  }

  for i, region := range c.Regions {
    output, err := os.Create(path.Join(countryPath, fmt.Sprintf("reg-%d", i)))
    if err != nil {
      panic(err)
    }

    output.WriteString(fmt.Sprintf("# %s region %d\n", c.Name, i))
    output.WriteString(fmt.Sprintf("%d %d\n", len(region.OuterVertices),
      len(region.Triangles)))
    output.WriteString("# Outer Vertices\n")
    for _, vertex := range region.OuterVertices {
      output.WriteString(fmt.Sprintf("%f,%f\n", vertex.X, vertex.Y))
    }
    output.WriteString("# Triangles\n")
    for _, triangle := range region.Triangles {
      output.WriteString(fmt.Sprintf("%d,%d,%d\n", triangle.T1, triangle.T2,
        triangle.T3))
    }

    output.Close()
  }

  writingMutex.Lock()
  fileList, err := os.OpenFile(path.Join(outputPath, "countrylist"),
    os.O_RDWR | os.O_APPEND, 0666)
  if err != nil {
    fileList, err = os.Create(path.Join(outputPath, "countrylist"))
  }
  if err != nil {
    panic(err)
  }
  fileList.WriteString(c.Name + "\n")
  fileList.Close()
  writingMutex.Unlock()
}

func WriteData(countries chan Country) {
  err := os.Mkdir(outputPath, 0755)
  if err != nil {
    panic(err)
  }
  ConvertCountries(countries)
}

// ConvertCountries takes a channel of countries and rewrites and
// triangles the data to form new data.
func ConvertCountries(countries chan Country) {
  var wg sync.WaitGroup
  for c := range countries {
    wg.Add(1)
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
      country.WriteBack()
      wg.Done()
    }(c)
  }
  wg.Wait()
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
  pwd, _ := os.Getwd()
  tmpDir, _ = ioutil.TempDir("", "gompile")
  os.Chdir(tmpDir)
  // Determine input path
  inputFile := path.Clean(path.Join(pwd, os.Args[1]))
  execPath = path.Join(pwd, path.Dir(os.Args[0]))

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
  var wg sync.WaitGroup
  wg.Add(1)
  go func() {
    ReadKML(kml, countries)
    close(countries)
    wg.Done()
  }()
  outputPath = path.Join(pwd, "share/countries")
  WriteData(countries)
  wg.Wait()
}
