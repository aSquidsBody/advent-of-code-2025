package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"sort"
	"strconv"
	"strings"
)

const INPUT_FILENAME = "input.txt"

func parseIdRange(line string) [2]int {
	var result [2]int = [2]int{}
	var err error

	split := strings.Split(line, "-")
	for i := range 2 {
		if result[i], err = strconv.Atoi(split[i]); err != nil {
			log.Fatalf("Could not parse int from string %s (%s). %s\n", split[i], line, err.Error())
		}
	}

	return result
}

func parseId(line string) int {
	if id, err := strconv.Atoi(line); err != nil {
		log.Fatalf("Could not parse int from string %s. %s\n", line, err.Error())
		return -1
	} else {
		return id
	}

}

func p1() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open file %s.", INPUT_FILENAME)
	}

	scanner := bufio.NewScanner(f)
	var idRanges = [][2]int{}
	var ids = []int{}
	var line string

	for scanner.Scan() {
		line = scanner.Text()
		if strings.Contains(line, "-") {
			idRanges = append(idRanges, parseIdRange(line))
		} else if len(line) > 0 {
			ids = append(ids, parseId(line))
		}
	}

	sort.Slice(idRanges, func(i, j int) bool {
		return idRanges[i][0] < idRanges[j][0]
	})

	var fCount int = 0
	for _, id := range ids {
		for _, idRange := range idRanges {
			if idRange[0] <= id && id <= idRange[1] {
				fCount++
				break
			}
		}
	}

	fmt.Printf("Part one: %d\n", fCount)
}

func p2() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open file %s.", INPUT_FILENAME)
	}

	scanner := bufio.NewScanner(f)
	var idRanges = [][2]int{}
	var line string

	for scanner.Scan() {
		line = scanner.Text()
		if strings.Contains(line, "-") {
			idRanges = append(idRanges, parseIdRange(line))
		}
	}

	sort.Slice(idRanges, func(i, j int) bool {
		return idRanges[i][0] < idRanges[j][0]
	})

	var idx int = 0
	var sum int = 0
	for idx < len(idRanges) {
		var combinedRange = [2]int{idRanges[idx][0], idRanges[idx][1]}
		idx++

		for idx < len(idRanges) && combinedRange[1] >= idRanges[idx][0] {
			combinedRange[1] = max(idRanges[idx][1], combinedRange[1])
			idx++
		}

		sum += 1 + combinedRange[1] - combinedRange[0]
	}

	fmt.Printf("Part two: %d\n", sum)
}

func main() {
	p1()
	p2()
}
