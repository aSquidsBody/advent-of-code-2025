package main

import (
	"bufio"
	"log"
	"os"
	"strconv"
	"strings"
)

const INPUT_FILENAME = "input.txt"

func p1() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open file %s.", INPUT_FILENAME)
	}

	scanner := bufio.NewScanner(f)
	var line string
	var rawRanges [][2]int = [][2]int{}

	// read the first half
	for scanner.Scan() {
		line = scanner.Text()
		if len(strings.Trim(line, " ")) == 0 {
			break
		}
		var split []string = strings.Split(line, "-")

		var rawRange [2]int = [2]int{}
		if d, err := strconv.Atoi(split[0]); err != nil {
			log.Fatalf("Could not parse string %s (%s). %s\n", split[0], line, err)
		} else {
			rawRange[0] = d
		}

		if d, err := strconv.Atoi(split[1]); err != nil {
			log.Fatalf("Could not parse string %s (%s). %s", split[1], line, err)
		} else {
			rawRange[1] = d
		}

		rawRanges = append(rawRanges, rawRange)

	}

	// read the second half
	for scanner.Scan() {
		line = scanner.Text()
	}
}

func main() {
	p1()
}
