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
  "os"
  "fmt"
  "io/ioutil"
  "path"
  "encoding/xml"
  "regexp"
)

var (
  pwd, tmpDir string
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

func ReadKML(kml *xmlKML, countries chan Country) {
  num := 0
  countries = make(chan Country)
  for i, folder := range kml.Document.Folders {
    for j, _ := range folder.Placemarks {
      num++
      go func(p, q int) {
        place := kml.Document.Folders[p].Placemarks[q]
        country := Country{Name: place.Name}
        // Unify polygon interfacing
        polygons := &place.MultiGeometry
        if len(place.MultiGeometry) == 0 {
          polygons = &[]xmlPolygon{place.Polygon}
        }
        country.Regions = make([]Region, len(*polygons))
        for r, _ := range *polygons {
          outer, inner := ExtractVertices(&(*polygons)[r])
          country.Regions[r].OuterVertices = outer
          country.Regions[r].InnerVertices = inner
        }
        countries <- country
      }(i, j)
    }
  }
  for i := 0; i < num; i++ {
    c := <-countries
    if c.Name == "Afghanistan" {
      fmt.Printf("Name: %s\nRegions (%d)\n", c.Name, len(c.Regions))
      for _, region := range c.Regions {
        fmt.Printf("\tOuter Boundary (%d):\n", len(region.OuterVertices))
        for _, vertex := range region.OuterVertices {
          fmt.Printf("\t\t%v\n", vertex)
        }
        for _, vertex := range region.InnerVertices {
          fmt.Printf("\t\t%v\n", vertex)
        }
      }
    }
  }
}

func ExtractVertices(p *xmlPolygon) ([]Vertex, []Vertex) {
  reg, _ := regexp.Compile(`-??[\d]+\.[\d]+,[\d]+\.[\d]+`)
  f := func(boundary xmlBoundaryIs) []Vertex {
    vertData := boundary.LinearRing.Coordinates
    matches := reg.FindAllString(vertData, -1)
    verts := make([]Vertex, len(matches))
    for i, match := range matches {
      fmt.Sscanf(match, "%f,%f", &verts[i].X, &verts[i].Y)
    }
    return verts
  }
  return f(p.OuterBoundaryIs), f(p.InnerBoundaryIs)
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
  inputFile := path.Clean(path.Join(pwd, path.Base(os.Args[1])))
  os.Chdir(tmpDir)

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
  // countries := make(chan Country)
  ReadKML(kml, nil)
}
