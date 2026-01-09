package main

import (
	"bufio"
	"fmt"
	"log"
	"math"
	"os"
	"slices"
	"strconv"
	"strings"
)

const INPUT_FILENAME = "input.txt"

type point struct {
	x, y int
}

func printPolygon(points []point) {
	var mx, my, MX, MY = math.MaxInt, math.MaxInt, 0, 0
	for _, p := range points {
		if p.x > MX {
			MX = p.x
		}
		if p.y > MY {
			MY = p.y
		}
		if p.x < mx {
			mx = p.x
		}
		if p.y < my {
			my = p.y
		}
	}

	MY++
	MX++
	var grid [][]byte = make([][]byte, 0, MX)
	for range MX {
		row := make([]byte, 0, MY)
		for range MY {
			row = append(row, '.')
		}
		grid = append(grid, row)
	}

	points = append(points, points[0])
	for i := range len(points) - 1 {
		for x := min(points[i].x, points[i+1].x); x <= max(points[i].x, points[i+1].x); x++ {
			for y := min(points[i].y, points[i+1].y); y <= max(points[i].y, points[i+1].y); y++ {
				grid[x][y] = '#'
			}
		}
	}

	for i := range len(grid) {
		for j := range len(grid[i]) {
			fmt.Printf(" %c", grid[i][j])
		}
		fmt.Println()
	}
}

func compressPoints(points []point) {
	xs := make([]int, 0, len(points))
	for _, point := range points {
		xs = append(xs, point.x)
	}
	slices.Sort(xs)

	ys := make([]int, 0, len(points))
	for _, point := range points {
		ys = append(ys, point.y)
	}
	slices.Sort(ys)

	xmap := map[int]int{}
	ymap := map[int]int{}
	i := 0
	for _, xVal := range xs {
		if _, ok := xmap[xVal]; !ok {
			xmap[xVal] = i
			i++
		}
	}

	i = 0
	for _, yVal := range ys {
		if _, ok := ymap[yVal]; !ok {
			ymap[yVal] = i
			i++
		}
	}

	for i := range points {
		points[i].x = xmap[points[i].x]
		points[i].y = ymap[points[i].y]
	}
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func rectangleArea(pt1, pt2 point) int {
	return (abs(pt1.x-pt2.x) + 1) * (abs(pt2.y-pt1.y) + 1)
}

func parseLine(line string) point {
	split := strings.Split(line, ",")

	var pt point = point{}
	var err error
	pt.x, err = strconv.Atoi(split[0])
	if err != nil {
		log.Fatalf("Failed to atoi %s\n", split[0])
	}
	pt.y, err = strconv.Atoi(split[1])
	if err != nil {
		log.Fatalf("Failed to atoi %s\n", split[1])
	}

	return pt
}

func p1() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Cannot open file. %s\n", err.Error())
	}

	// array of points rather than array of pointers
	points := make([]point, 0, 1024)
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		points = append(points, parseLine(scanner.Text()))
	}
	fmt.Printf("read %d points from file\n", len(points))

	var result = 0
	for _, first := range points {
		for _, second := range points {
			result = max(result, rectangleArea(first, second))
		}
	}

	fmt.Printf("Part one: %d\n", result)
}

func p2() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Cannot open file. %s\n", err.Error())
	}

	// array of points rather than array of pointers
	points := make([]point, 0, 1024)
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		points = append(points, parseLine(scanner.Text()))
	}
	fmt.Printf("read %d points from file\n", len(points))

	// printPolygon(points)

	fmt.Println()
	// compressPoints(points)
	// printPolygon(points)

	var result = 0
	fmt.Printf("Part two: %d\n", result)
}

func main() {
	p1()
	p2()
}
