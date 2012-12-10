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
  "archive/zip"
  "bufio"
  "encoding/binary"
  "log"
  "fmt"
  "os"
  "os/exec"
  "io/ioutil"
  "io"
  "encoding/xml"
  "bytes"
  "regexp"
  "math/rand"
  "math"
  "path"
)

var (
  numCountries = 0
  pwd, tmpDir string
)

type (
  vertex struct {
    x, y float64
  }

  country struct {
    name string
    polygons int
  }

  // XML struct
  BoundaryIs struct {
    LinearRing struct {
      Coordinates string `xml:"coordinates"`
    }
  }

  // XML struct
  Polygon struct {
    OuterBoundaryIs BoundaryIs `xml:"outerBoundaryIs"`
    InnerBoundaryIs BoundaryIs `xml:"innerBoundaryIs"`
  }

  // XML struct
  Place struct {
    Description string `xml:"description"`
    Name string `xml:"name"`
    Style struct {
      PolyStyle struct {
        Color string `xml:"color"`
        ColorMode string `xml:"colorMode"`
      }
    }
    MultiGeometry []Polygon `xml:"MultiGeometry>Polygon"`
    Polygon Polygon `xml:"Polygon"`
  }

  // XML struct
  KML struct {
    XMLName xml.Name `xml:"kml"`
    Document struct {
      Name string `xml:"name"`
      Folders []struct {
        Name string `xml:"name"`
        Placemarks []Place `xml:"Placemark"`
      } `xml:"Folder"`
    }
  }
)


// Opens the zip and return the entire byte-array.
func OpenZip (fn string) ([]byte, error) {
  r, err := zip.OpenReader(fn)
  if err != nil {
    return nil, err
  }
  defer r.Close()

  var b []byte
  for _, f := range r.File {
    fmt.Printf("Deflating %s...\n", f.Name)
    rc, err := f.Open()
    if err != nil {
      return nil, err
    }
    b, err = ioutil.ReadAll(rc)
    if err != nil {
      return nil, err
    }
    rc.Close()
  }
  return b, nil
}

// For a collection of vertices, determineHole determines a random point that
// is in that vertex. Used for determining holes.
func determineHole(vertices []vertex) (float64, float64) {
  min := vertices[0]
  max := vertices[0]
  // Determine box.
  for _, v := range vertices {
    if v.x < min.x {
      min.x = v.x
    }
    if v.y < min.y {
      min.y = v.y
    }
    if v.x > max.x {
      max.x = v.x
    }
    if v.y > max.y {
      max.y = v.y
    }
  }
  // Pick the points at random and check if they are in the polygon.
  var x, y float64
  for {
    x, y = rand.Float64(), rand.Float64()
    x = (x * math.Abs(max.x - min.x)) + min.x
    y = (y * math.Abs(max.y - min.y)) + min.y
    if pointInPolygon(vertex{x, y}, vertices) {
      break
    }
  }
  return x, y
}

// pointInPolygon determines if a certain point is in a polygon.
func pointInPolygon (point vertex, vertices []vertex) bool {
  yes := false
  for i, j := 0, len(vertices)-1; i < len(vertices); j, i = i, i+1 {
    if ((vertices[i].y > point.y) != (vertices[j].y > point.y) &&
      (point.x < (vertices[j].x-vertices[i].x) * (point.y - vertices[i].y /
      (vertices[j].y - vertices[i].y) + vertices[i].x))) {
      yes = !yes
    }
  }
  return yes
}

// PolygonData writes to a byte array the .poly-file that belongs to the vertex
// data.
func PolygonData(p *Polygon) []byte {
  buf := new(bytes.Buffer)
  reg, _ := regexp.Compile("-??[0-9.]+,-??[0-9.]+,-??[0-9]+")

  // Get the numbers.
  matchOuter := reg.FindAllString(p.OuterBoundaryIs.LinearRing.Coordinates, -1)
  matchInner := reg.FindAllString(p.InnerBoundaryIs.LinearRing.Coordinates,
    -1)
  size := len(matchOuter) + len(matchInner)

  buf.WriteString(fmt.Sprintf("%d 2 0 0\n", size))

  // Functions for writing the vertices for the inner and outer boundary.
  getCoord := func(results []string) (vertices []vertex) {
    vertices = make([]vertex, len(results))
    for i, str := range results {
      fmt.Sscanf(str, "%f,%f,%f", &(vertices[i].x), &(vertices[i].y))
    }
    return vertices
  }
  writeCoord := func(vertices []vertex, offset int) {
    for i, vertex := range vertices {
      q := offset+i
      buf.WriteString(fmt.Sprintf("%d %f %f\n", q, vertex.x, vertex.y))
    }
  }
  writeEdges := func(vertices []vertex, offset int) {
    for i, _ := range vertices {
      q := offset+i+1
      if i == len(vertices)-1 {
        q = offset
      }

      buf.WriteString(fmt.Sprintf("%d %d %d\n", i+offset, i+offset, q))
    }
  }

  outerVert := getCoord(matchOuter)
  innerVert := getCoord(matchInner)

  // Write everything.
  buf.WriteString("# Outer boundary\n")
  writeCoord(outerVert, 0)
  buf.WriteString("# Inner boundary\n")
  writeCoord(innerVert, len(outerVert))
  buf.WriteString(fmt.Sprintf("%d\n", size))
  buf.WriteString("# Outer boundary edges\n")
  writeEdges(outerVert, 0)
  buf.WriteString("# Inner boundary edges\n")
  writeEdges(innerVert, len(outerVert))
  buf.WriteString("# Number of holes\n")
  if len(innerVert) == 0 {
    buf.WriteString("0\n")
  } else {
    buf.WriteString("1\n")
    holeX, holeY := determineHole(innerVert)
    buf.WriteString(fmt.Sprintf("0 %f %f\n", holeX, holeY))
  }
  return buf.Bytes()
}

// Triangulate calls an external command written by someone else to triangulate
// the set of vertices.
func Triangulate(countries chan country, done chan country) {
  country := <-countries
  dirName := tmpDir+"/countries/" + country.name + "/"
  fmt.Printf("Triangulating %s\n", country.name)
  for i := 0; i < country.polygons; i++ {
    fileName := fmt.Sprintf("poly-%d.poly", i)
    command := exec.Command(pwd+"/bin/triangle", dirName + fileName)
    std := new(bytes.Buffer)
    command.Stderr, command.Stdout = std, std
    err := command.Run()
    if err != nil {
      fmt.Fprintf(os.Stderr, "Command failed for %s: %s\n", dirName + fileName,
        err)
      if std.Len() > 0 {
        std.WriteTo(os.Stderr)
      }
      break
    }
    os.Remove(dirName + fileName)
  }
  done <- country
}

// Compile uses the list of countries and the outputs of Triangle to make a 
// binary file containing the correct data.
func Compile(countries []country) {
  out, err := os.Create(pwd+"/share/out.dat")
  if err != nil {
    log.Fatal(err)
  }
  defer out.Close()

  // Read the triangulated edge and node-files.
  regVert, _ := regexp.Compile("-??\\d+(.\\d+)?")
  regEdge, _ := regexp.Compile("\\d+")

  binary.Write(out, binary.LittleEndian, uint16(len(countries)))
  for _, country := range countries {
    binary.Write(out, binary.LittleEndian, uint16(len(country.name)))
    out.Write([]byte(country.name))
    binary.Write(out, binary.LittleEndian, uint16(country.polygons))
    for i := 0; i < country.polygons; i++ {
      fileName := fmt.Sprintf(tmpDir+"/countries/%s/poly-%d.1", country.name, i)
      // Get vertices
      vertFile, err := os.Open(fileName + ".node")
      if err != nil {
        log.Fatal(err)
      }

      var size uint32

      buf := bufio.NewReader(vertFile)
      line, err := buf.ReadString('\n')
      fmt.Sscanf(line, "%d", &size)
      binary.Write(out, binary.LittleEndian, size)
      for err != io.EOF {
        line, err = buf.ReadString('\n')
        match := regVert.FindAllString(line, -1)
        if len(match) == 4 && line[0] != '#' {
          var x, y float64
          fmt.Sscanf(match[1], "%f", &x)
          fmt.Sscanf(match[2], "%f", &y)
          binary.Write(out, binary.LittleEndian, x)
          binary.Write(out, binary.LittleEndian, y)
        }
      }
      vertFile.Close()
      // Get edges
      edgeFile, err := os.Open(fileName + ".ele")
      if err != nil {
        log.Fatal(err)
      }
      buf = bufio.NewReader(edgeFile)
      line, err = buf.ReadString('\n')
      fmt.Sscanf(line, "%d", &size)
      binary.Write(out, binary.LittleEndian, size)
      for err != io.EOF {
        line, err = buf.ReadString('\n')
        match := regEdge.FindAllString(line, -1)
        if len(match) == 4 && line[0] != '#' {
          var x, y, z uint32
          fmt.Sscanf(match[1], "%d", &x)
          fmt.Sscanf(match[2], "%d", &y)
          fmt.Sscanf(match[3], "%d", &z)
          binary.Write(out, binary.LittleEndian, x)
          binary.Write(out, binary.LittleEndian, y)
          binary.Write(out, binary.LittleEndian, z)
        }
      }
      edgeFile.Close()
    }
  }
}

// Convert takes the unmarshalled KML data to determine the countries and their
// polygons.
func Convert (kml *KML) {
  os.Mkdir(tmpDir+"/countries", 0755)
  for i, _:= range kml.Document.Folders {
    for _, _ = range kml.Document.Folders[i].Placemarks {
      numCountries++
    }
  }
  countries := make(chan country, numCountries)
  done := make(chan country, numCountries)
  for _, folder := range kml.Document.Folders {
    for _, place := range folder.Placemarks {
      // Each country can run concurrently.
      go func(place Place) {
        fmt.Printf("Creating countries/%s\n", place.Name)
        os.Mkdir(fmt.Sprintf(tmpDir+"/countries/%s", place.Name), 0755)

        writePolygon := func(p *Polygon, n int) {
          out, _ := os.Create(fmt.Sprintf(tmpDir+"/countries/%s/poly-%d.poly",
            place.Name, n))
          out.Write(PolygonData(p))
          out.Close()
        }
        ctry := country{name: place.Name}
        if len(place.MultiGeometry) > 0 {
          for i, _ := range place.MultiGeometry {
            writePolygon(&place.MultiGeometry[i], i)
          }
          ctry.polygons = len(place.MultiGeometry)
        } else {
          writePolygon(&place.Polygon, 0)
          ctry.polygons = 1
        }
        countries <- ctry
        Triangulate(countries, done)
      }(place)
    }
  }
  // Although each country can be read and written concurrently, we need to make
  // sure each file has been finished before we can compile the data.
  c := make([]country, numCountries)
  for i := 0; i < numCountries; i++ {
    c[i] = <-done
  }
  Compile(c)
}

func main () {
  if len(os.Args) < 2 {
    log.Fatalln("Not enough arguments to commandline")
  }
  pwd, _ = os.Getwd()
  tmpDir, _ = ioutil.TempDir("", "gompile")
  countriesFile := os.Args[1]
  if !path.IsAbs(countriesFile) {
    countriesFile = path.Clean(path.Join(pwd, os.Args[1]))
  }
  os.Chdir(tmpDir)
  fmt.Printf("Unzipping %s\n", countriesFile)
  b, err := OpenZip(countriesFile)
  if err != nil {
    log.Fatal(err)
  }
  fmt.Printf("Unmarshalling %s...\n", countriesFile)
  kml := KML{}
  err = xml.Unmarshal(b, &kml)
  if err != nil {
    log.Fatal(err)
  }

  Convert(&kml)

  os.Chdir(pwd)
  // os.RemoveAll(tmpDir)
}
