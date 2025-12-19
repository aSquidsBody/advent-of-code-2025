package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
)

const INPUT_FILE = "input.txt"

func readMatrixFile(filename string) [][]byte {
	var input [][]byte = [][]byte{}

	file, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		var read []byte = make([]byte, len(scanner.Bytes()))
		copy(read, scanner.Bytes())

		input = append(input, read)
	}

	var result [][]byte = make([][]byte, len(input)+2)
	for i := range result {
		result[i] = make([]byte, len(input[0])+2)
		for j := range result[i] {
			result[i][j] = '.'
		}
	}

	for i, slice := range input {
		copy(result[i+1][1:], slice)
	}

	return result
}

func hasNNeighbors(row, col, n int, matrix [][]byte) bool {
	var numNeigbors int = 0

	var neighborIndices [][]int = [][]int{
		{row + 1, col + 1},
		{row + 1, col},
		{row + 1, col - 1},
		{row, col + 1},
		{row, col - 1},
		{row - 1, col + 1},
		{row - 1, col},
		{row - 1, col - 1},
	}

	for _, neighbors := range neighborIndices {
		if matrix[neighbors[0]][neighbors[1]] == '@' {
			numNeigbors++
		}
	}

	return numNeigbors >= n
}

func clearRolls(matrix [][]byte) int {
	var removeIndices [][2]int = [][2]int{}

	var sum int = 0
	for row := 1; row < len(matrix)-1; row++ {
		for col := 1; col < len(matrix)-1; col++ {
			if matrix[row][col] == '@' && !hasNNeighbors(row, col, 4, matrix) {
				removeIndices = append(removeIndices, [2]int{row, col})
				sum += 1
			}
		}
	}

	var copyMatrix [][]byte = make([][]byte, len(matrix))
	for row := range matrix {
		copyMatrix[row] = make([]byte, len(matrix[row]))
		copy(copyMatrix[row], matrix[row])
	}

	for _, k := range removeIndices {
		matrix[k[0]][k[1]] = '.'
		copyMatrix[k[0]][k[1]] = 'x'
	}

	// printMatrix(copyMatrix)

	return sum
}

func printMatrix(matrix [][]byte) {
	for row := range matrix {
		for col := range matrix[row] {
			fmt.Printf("%s", string(matrix[row][col]))
		}
		fmt.Println()
	}
}

func min(x, y int) int {
	if x < y {
		return x
	}

	return x
}

func p1(filename string) {
	matrix := readMatrixFile(filename)

	copyMatrix := make([][]byte, len(matrix))
	for i := range matrix {
		copyMatrix[i] = make([]byte, len(matrix[i]))
		copy(copyMatrix[i], matrix[i])
	}

	var sum int = 0
	for row := 1; row < len(matrix)-1; row++ {
		for col := 1; col < len(matrix)-1; col++ {
			if matrix[row][col] == '@' && !hasNNeighbors(row, col, 4, matrix) {
				copyMatrix[row][col] = 'x'
				sum += 1
			}
		}
	}
	fmt.Printf("Part one: %d\n", sum)
}

func p2(filename string) {

	matrix := readMatrixFile(filename)
	// printMatrix(matrix)

	var sum int = 0
	var numCleared int = 1
	for numCleared != 0 {
		numCleared = clearRolls(matrix)

		sum += numCleared
	}

	fmt.Printf("Part two %d\n", sum)
}

func main() {
	p1(INPUT_FILE)
	p2(INPUT_FILE)
}
