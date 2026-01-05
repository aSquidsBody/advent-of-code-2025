package main

import (
	"bufio"
	"container/heap"
	"fmt"
	"log"
	"os"
	"slices"
	"strconv"
	"strings"
)

var points []point
var dHeap distHeap
var circuits []*circuit

type circuit map[*point]bool // hashset of points

func (c *circuit) add(pt *point) {
	(*c)[pt] = true
	pt.circ = c
}

func (c *circuit) remove(pt *point) {
	if pt.circ == c {
		pt.circ = nil
	}
	delete(*c, pt)
}

func saveCircuit(c *circuit) {
	circuits = append(circuits, c)
}

func mergeCircuits(cPtr1, cPtr2 *circuit) {
	idx1 := slices.Index(circuits, cPtr1)
	idx2 := slices.Index(circuits, cPtr2)

	if idx1 > idx2 {
		mergeCircuits(cPtr2, cPtr1)
		return
	}

	if idx1 < 0 || idx2 < 0 {
		log.Fatalf("Encountered circuit which is not in the list")
	}

	// move each point from cIdx2 into cIdx1
	for pt := range *cPtr2 {
		cPtr1.add(pt)
		cPtr2.remove(pt)
	}

	// delete cIdx2
	circuits[idx2] = circuits[len(circuits)-1] // replace with last elem
	circuits = circuits[:len(circuits)-1]      // shorten circuits

}

type distHeap []pointPair

func (dh distHeap) Len() int {
	return len(dh)
}

func (dh distHeap) Less(x, y int) bool {
	return dh[x].distance < dh[y].distance
}

func (dh distHeap) Swap(x, y int) {
	dh[x], dh[y] = dh[y], dh[x]
}

func (dh *distHeap) Push(x any) {
	*dh = append(*dh, x.(pointPair))
}

func (dh *distHeap) Pop() any {
	old := *dh
	n := len(old)
	x := old[n-1]
	*dh = old[0 : n-1]
	return x
}

type point struct {
	x    int
	y    int
	z    int
	circ *circuit
}

type pointPair struct {
	pt1, pt2 *point
	distance int
}

func dist(x, y point) int {
	return (x.x-y.x)*(x.x-y.x) + (x.y-y.y)*(x.y-y.y) + (x.z-y.z)*(x.z-y.z)
}

const INPUT_FILENAME = "input.txt"
const FILE_BUFFER_SIZE = 32768

func readPoints(f *os.File, points []point) []point {
	scanner := bufio.NewScanner(f)
	idx := 0
	for scanner.Scan() {
		var point = point{}
		var err error

		nList := strings.Split(scanner.Text(), ",")

		point.x, err = strconv.Atoi(nList[0])
		if err != nil {
			log.Fatalf("Failed to atoi %s\n", nList[0])
		}
		point.y, err = strconv.Atoi(nList[1])
		if err != nil {
			log.Fatalf("Failed to atoi %s\n", nList[1])
		}
		point.z, err = strconv.Atoi(nList[2])
		if err != nil {
			log.Fatalf("Failed to atoi %s\n", nList[2])
		}
		point.circ = nil
		idx++

		points = append(points, point)
	}

	return points
}

func initDistHeap(points []point) {
	var size int = len(points)
	dHeap = make([]pointPair, 0, size*size/2)
	for i := range size {
		for j := i + 1; j < size; j++ {
			dHeap = append(dHeap, pointPair{&points[i], &points[j], dist(points[i], points[j])})
		}
	}

	heap.Init(&dHeap)
}

func handleSameCircuit(pt1, pt2 *point) int {
	if pt1.circ == nil {
		// not part of any circuits, so create a new one
		c := make(circuit, 0)
		c.add(pt1)
		c.add(pt2)
		saveCircuit(&c)
		return 2
	}

	// do nothing since they are part of the same circuit
	return 0
}

func handleSepCircuits(pt1, pt2 *point) int {
	if pt2.circ == nil {
		return handleSepCircuits(pt2, pt1) // always make pt2 not-nil
	}

	if pt1.circ == nil {
		// pt1 is not part of any circuits, and so it must be added to pt2's circuit
		pt1.circ = pt2.circ
		(*pt1.circ)[pt1] = true
		return 1
	}

	// pt1 and pt2 are part of different not-null circuits, so they must be merged
	mergeCircuits(pt1.circ, pt2.circ)
	return 0
}

func p1() {
	file, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open %s. %s\n", INPUT_FILENAME, err.Error())
	}
	defer file.Close()
	points = readPoints(file, make([]point, 0, 1024))
	initDistHeap(points)
	circuits = make([]*circuit, 0, 200)

	for range 1000 {
		var shortest pointPair = heap.Pop(&dHeap).(pointPair)
		var pt1 *point = shortest.pt1
		var pt2 *point = shortest.pt2

		if pt1.circ == pt2.circ {
			handleSameCircuit(pt1, pt2)
		} else {
			handleSepCircuits(pt1, pt2)
		}
	}

	var threeLargest = [3]int{0, 0, 0}
	for _, c := range circuits {
		l := len(*c)

		if l > threeLargest[0] {
			threeLargest[2] = threeLargest[1]
			threeLargest[1] = threeLargest[0]
			threeLargest[0] = l
		} else if l > threeLargest[1] {
			threeLargest[2] = threeLargest[1]
			threeLargest[1] = l
		} else if l > threeLargest[2] {
			threeLargest[2] = l
		}
	}

	var result = 1
	for _, v := range threeLargest {
		result *= v
	}

	fmt.Printf("Part one: %d\n", result)
}

func p2() {
	file, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open %s. %s\n", INPUT_FILENAME, err.Error())
	}
	defer file.Close()
	points = readPoints(file, make([]point, 0, 1024))
	initDistHeap(points)
	circuits = make([]*circuit, 0, 200)

	var nConns = 0 // number of points that are connected
	var pt1, pt2 *point
	for nConns < len(points) {
		var shortest pointPair = heap.Pop(&dHeap).(pointPair)
		pt1 = shortest.pt1
		pt2 = shortest.pt2

		if pt1.circ == pt2.circ {
			nConns += handleSameCircuit(pt1, pt2)
		} else {
			nConns += handleSepCircuits(pt1, pt2)
		}
	}

	var total = pt1.x * pt2.x
	fmt.Printf("Part one: %d\n", total)
}

func main() {
	p1()
	p2()
}
