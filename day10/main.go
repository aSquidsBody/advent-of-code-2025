package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"strings"
)

// input format
// [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}

// The manual describes one machine per line. Each line contains a single indicator light diagram in [square brackets], one or more button wiring schematics in (parentheses), and joltage requirements in {curly braces}.

type indicatorLightDiagram uint // 0 bit represents that light should be off, and 1 bit represents that light should be on
type buttonSchematic uint       // use the bits to indicate which lights are controlled by the button
type joltage int

type machine struct {
	numLights    uint
	lights       indicatorLightDiagram
	buttons      []buttonSchematic // some number of buttons
	requirements []joltage         // num joltage requirements == num lights
}

func assert(expr bool, message ...string) {
	if !expr {
		if len(message) > 0 {
			log.Printf("[FATAL] %s\n", message[0])
		}
		panic(expr)
	}

}

func parseLine(line string) machine {
	m := machine{}

	fields := strings.Fields(line)
	assert(len(fields) > 0)

	m.numLights = uint(len(fields[0]) - 2)

	m.lights = 0
	var i uint
	for i = 0; i < m.numLights; i++ {
		if fields[0][i+1] == '#' {
			m.lights += (1 << i)
		}
	}
}

const INPUT_FILENAME = "input.txt"

func p1() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open file. %s\n", err.Error())
	}

	var total int = 0

	var scanner *bufio.Scanner = bufio.NewScanner(f)
	for scanner.Scan() {
		// total +=
	}

	fmt.Printf("Part one: %d\n", total)
}

func main() {

}
