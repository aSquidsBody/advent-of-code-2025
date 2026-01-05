package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strconv"
	"strings"
)

const INPUT_FILENAME = "input.txt"

func productCol(cIdx int, matrix [][]string) int {
	var total = 1
	for _, row := range matrix {
		n, err := strconv.Atoi(row[cIdx])
		if err != nil {
			log.Fatal(err)
		}
		total *= n
	}

	return total
}

func sumCol(cIdx int, matrix [][]string) int {
	var total = 0
	for _, row := range matrix {
		n, err := strconv.Atoi(row[cIdx])
		if err != nil {
			log.Fatal(err)
		}
		total += n
	}

	return total
}

func applyOperation(cIdx int, matrix [][]string) int {
	switch matrix[len(matrix)-1][cIdx] {
	case "*":
		return productCol(cIdx, matrix[0:len(matrix)-1])
	case "+":
		return sumCol(cIdx, matrix[0:len(matrix)-1])
	}

	return 0
}

func p1() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Error openinng file %s. %s", INPUT_FILENAME, err.Error())
	}
	defer f.Close()

	var input [][]string = [][]string{}
	var scanner *bufio.Scanner = bufio.NewScanner(f)
	for scanner.Scan() {
		input = append(input, strings.Fields(scanner.Text()))
	}

	var total = 0
	for cIdx := range len(input[0]) {
		total += applyOperation(cIdx, input)
	}

	fmt.Printf("Part one: %d\n", total)
}

func applyProduct(operands []int) int {
	var total = 1
	for i := range operands {
		total *= operands[i]
	}

	return total
}

func applySum(operands []int) int {
	var total = 0
	for i := range operands {
		total += operands[i]
	}

	return total
}

func applyOperationP2(op byte, operands []int) int {
	if op == '*' {
		return applyProduct(operands)
	}

	return applySum(operands)
}

func p2() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Error openinng file %s. %s", INPUT_FILENAME, err.Error())
	}
	defer f.Close()

	var input [][]byte = [][]byte{}
	var scanner *bufio.Scanner = bufio.NewScanner(f)
	for scanner.Scan() {
		input = append(input, []byte(scanner.Text()))
	}

	var total = 0
	var operation byte = 0
	var numStr string
	var operands []int = []int{}
	for cIdx := range len(input[0]) {
		numStr = ""

		if input[len(input)-1][cIdx] != ' ' {
			operation = input[len(input)-1][cIdx]
		}

		for rIdx := range len(input) - 1 {
			numStr = fmt.Sprintf("%s%s", numStr, string(input[rIdx][cIdx]))
		}

		numStr = strings.TrimSpace(numStr)
		if len(numStr) == 0 {
			total += applyOperationP2(operation, operands)
			operands = []int{}
			operation = 0
			continue
		}

		n, err := strconv.Atoi(numStr)
		if err != nil {
			log.Fatal(err)
		}

		operands = append(operands, n)
		// fmt.Println(operands)
	}
	// do a final calculation as well
	total += applyOperationP2(operation, operands)

	fmt.Printf("Part two: %d\n", total)
}

func main() {
	p1()
	p2()
}
