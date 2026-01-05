package main

import (
	"fmt"
	"io"
	"log"
	"os"
	"strconv"
	"strings"
)

const INPUT_FILE = "input.txt"

func min(x, y int) int {
	if x < y {
		return x
	}

	return y
}

func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

func numDigits(x int) uint8 {
	var digits uint8 = 0
	x = abs(x)

	for {
		x /= 10
		digits++

		if x == 0 {
			break
		}
	}

	return digits
}

func pow(x int, exp uint8) int {
	result := 1

	for _ = range exp {
		result *= x
	}

	return result
}

func parseInputFile(filename string) [][]int {
	file, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	b, err := io.ReadAll(file)
	if err != nil {
		log.Fatal(err)
	}

	var split []string = strings.Split(strings.Trim(string(b), "\n"), ",")
	var result [][]int = make([][]int, len(split))
	for idx, val := range split {

		var nums []string = strings.Split(val, "-")
		start, err := strconv.Atoi(nums[0])
		if err != nil {
			log.Fatal(err)
		}
		end, err := strconv.Atoi(nums[1])
		if err != nil {
			log.Fatal(err)
		}

		result[idx] = []int{start, end}
	}

	return result

}

func getMaxInput(input [][]int) int {
	var max int = 0
	for _, pair := range input {
		if max < pair[1] {
			max = pair[1]
		}
	}

	return max
}

func numInInput(num int, input [][]int) bool {
	for _, inputTuple := range input {
		if inputTuple[0] <= num && num <= inputTuple[1] {
			// fmt.Printf("   %d < %d < %d ", inputTuple[0], num, inputTuple[1])
			return true
		}
	}

	return false
}

func repeatInt(i int, n uint8) string {
	str := ""
	for range n {
		str = fmt.Sprintf("%s%d", str, i)
	}

	return str
}

func partOne() {
	var input [][]int = parseInputFile(INPUT_FILE) // read input ints in list
	var max int = getMaxInput(input)               // get the largest int in the file
	var nDigits uint8 = numDigits(max)

	var totalSum int = 0

	for i := range pow(10, nDigits/2) {

		repeatNum, err := strconv.Atoi(repeatInt(i, 2))
		if err != nil {
			log.Fatal(err)
		}

		if numInInput(repeatNum, input) {
			totalSum += repeatNum
		}

	}
	fmt.Printf("Part one: %d\n", totalSum)
}

func partTwo() {
	var input [][]int = parseInputFile(INPUT_FILE) // read input ints in list
	var max int = getMaxInput(input)               // get the largest int in the file
	var nDigits uint8 = numDigits(max)

	var totalSum int = 0

	var seen map[int]bool = make(map[int]bool)
	for numRepeats := uint8(2); numRepeats <= nDigits; numRepeats++ {
		for i := range pow(10, nDigits) {
			repeatNum, err := strconv.Atoi(repeatInt(i, numRepeats))
			// fmt.Printf("%d", repeatNum)
			if err != nil {
				log.Fatal(err)
			}

			if repeatNum > max {
				break
			}
			// 9,697,040,132
			if !seen[repeatNum] && numInInput(repeatNum, input) {
				// fmt.Printf(" invalid")
				seen[repeatNum] = true
				totalSum += repeatNum
			}
			// fmt.Print("\n")
		}
	}

	fmt.Printf("Part two: %d\n", totalSum)
}

func main() {
	partOne()
	partTwo()
}
