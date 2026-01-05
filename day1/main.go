package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
)

const INPUT_FILE = "input.txt"
const START_LOCATION = 50
const DIAL_DOMAIN = 100 // 100 dial values between 0-99

func mod(x, y int) int {
	if x >= 0 {
		return x % y
	}

	return ((x % y) + y) % y
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

// spinDial computes the new location of the dial given a starting location
// and a delta
func spinDial(start, delta int) int {
	return mod((start + delta), DIAL_DOMAIN)
}

func getCrossZeros(start, delta int) int {
	if delta == 0 {
		return 0
	}

	result := (start + delta) / DIAL_DOMAIN
	if delta > 0 {
		return result
	}

	result = abs(result)
	if start != 0 && start+delta <= 0 {
		result += 1
	}

	// start at 75
	// add 35 -> 120 so cross once

	// how do I know that?
	// 1. 35 // 100 == 0 and
	// 2. 120 // 100 == 1 so total of one crosses

	// edge cases:
	// 1. 35 - 35 == 0 -> cross once. Check that start + delta == 0
	// 2. 35 - 100 == -75 -> cross once, -75 // 100 == -1, so need to abs value the result
	// 3. 35 + 65 == 100 -> 100//100 == 1, so this works
	// 4. 35 - 135 == -100 == 0, so crosses once to get to negative and crosses a second time to get to -100
	return result
}

// parseRotationString parses a string like 'R25' and returns an integer delta
// represented by the input
func parseRotationString(rotString string) (delta int) {
	if len(rotString) <= 1 {
		log.Fatal(fmt.Sprintf("rotation string %s is invalid", rotString))

		return
	}

	rotInt, err := strconv.Atoi(rotString[1:])
	if err != nil {
		log.Fatal(fmt.Sprintf("rotation string %s is invalid", rotString))
		return
	}

	if rotString[0] == 'R' {
		return rotInt
	}

	if rotString[0] == 'L' {
		return -rotInt
	}

	log.Fatal(fmt.Sprintf("rotation string %s is invalid", rotString))
	return 0
}

func partOne() {
	// initialize
	location := START_LOCATION
	inputFile := INPUT_FILE
	counter := 0

	// main functionality ...
	file, err := os.Open(inputFile)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		rotString := scanner.Text()
		location = spinDial(location, parseRotationString(rotString))
		if location == 0 {
			counter += 1
		}
	}

	fmt.Printf("Part one: %d\n", counter)
}

func partTwo() {
	// initialize
	location := START_LOCATION
	inputFile := INPUT_FILE
	counter := 0

	// main functionality ...
	file, err := os.Open(inputFile)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		rotString := scanner.Text()

		delta := parseRotationString(rotString)
		num := getCrossZeros(location, delta)
		counter += num
		location = spinDial(location, delta)
	}

	fmt.Printf("Part two: %d\n", counter)
}

func main() {
	partOne()

	partTwo()
}
