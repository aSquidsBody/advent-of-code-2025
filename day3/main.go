package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
)

const INPUT_FILE = "input.txt"

const REQUIRED_DIGITS = 12

func deleteAtIdx(idx int, slice []byte) {
	copy(slice[idx:], slice[idx+1:])
}

func concatMaxN(line []byte, requiredLength int) int {
	// current best sub-int
	var current []byte = make([]byte, requiredLength)
	copy(current, line[0:requiredLength])
	currentValue, err := strconv.Atoi(string(current))
	if err != nil {
		log.Fatal(err)
	}

	var bestCandidateValue int = currentValue
	var bestCandidate []byte = make([]byte, requiredLength)
	var candidate []byte = make([]byte, requiredLength)
	for nextIdx := requiredLength; nextIdx < len(line); nextIdx++ {

		for deleteIdx := range requiredLength {
			copy(candidate, current)
			deleteAtIdx(deleteIdx, candidate)
			candidate[requiredLength-1] = line[nextIdx]

			candidateValue, err := strconv.Atoi(string(candidate))
			if err != nil {
				log.Fatal(err)
			}

			if candidateValue >= bestCandidateValue {
				bestCandidateValue = candidateValue
				copy(bestCandidate, candidate)
			}
		}

		if bestCandidateValue >= currentValue {
			currentValue = bestCandidateValue
			copy(current, bestCandidate)
		}
	}

	return currentValue
}

func concatMaxTwo(line []byte) int {
	var max int = 0

	for s := 0; s < len(line)-1; s++ {
		for e := s + 1; e < len(line); e++ {
			var candidate int = 10*int(line[s]-'0') + int(line[e]-'0')
			if candidate > max {
				max = candidate
			}
		}
	}
	return max
}

func p1(filename string) {
	var totalJoltage int = 0

	file, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		totalJoltage += concatMaxTwo(scanner.Bytes())
	}

	fmt.Println(totalJoltage)
}

func p2(filename string) {
	var totalJoltage int = 0

	file, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		totalJoltage += concatMaxN(scanner.Bytes(), REQUIRED_DIGITS)
	}

	fmt.Println(totalJoltage)
}

func main() {
	p1(INPUT_FILE)
	p2(INPUT_FILE)
}
