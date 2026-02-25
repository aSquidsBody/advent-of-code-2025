package main

import (
	"bufio"
	"fmt"
	"log"
	"math"
	"os"
	"strconv"
	"strings"
)

// The manual describes one machine per line. Each line contains a single indicator light diagram in [square brackets], one or more button wiring schematics in (parentheses), and joltage requirements in {curly braces}.

type machine struct {
	numLights    uint   // integer number of lights
	lights       uint   // 0 bit represents that light should be off, and 1 bit represents that light should be on
	buttons      []uint // use the bits to indicate which lights are controlled by the button
	requirements []uint // num joltage requirements == num lights
}

func printMachine(m *machine) {
	fmt.Printf("Machine:\n")
	fmt.Printf("%d, %d\n", m.numLights, m.lights)
	fmt.Printf("%+v\n", m.buttons)
	fmt.Printf("%+v\n", m.requirements)
}

func assert(expr bool, message ...string) {
	if !expr {
		if len(message) > 0 {
			log.Printf("[FATAL] %s\n", message[0])
		}
		panic(expr)
	}
}

func powTwo(exp int) int {
	var result int = 2
	for range exp {
		result = result << 1
	}

	return result
}

func populateLights(m *machine, lightStr string) {
	lightStr = lightStr[1 : len(lightStr)-1]
	m.numLights = uint(len(lightStr))
	m.lights = uint(0)

	for i, b := range lightStr {
		if b == '#' {
			m.lights += (1 << i)
		}
	}
}

func populateButtons(m *machine, buttonStrs []string) {
	assert(m.numLights != 0)
	m.buttons = make([]uint, 0, 32)

	for btnIdx, buttonStr := range buttonStrs {
		m.buttons = append(m.buttons, uint(0))

		buttonStr = buttonStr[1 : len(buttonStr)-1]
		for digit := range strings.SplitSeq(buttonStr, ",") {
			d, err := strconv.Atoi(digit)
			if err != nil {
				log.Fatalf("failed to convert digit %s\n", digit)
			}
			m.buttons[btnIdx] += (1 << d)
		}
	}
}

func populateRequirements(m *machine, reqStr string) {
	m.requirements = make([]uint, 0, m.numLights)

	reqStr = reqStr[1 : len(reqStr)-1]
	for digit := range strings.SplitSeq(reqStr, ",") {
		d, err := strconv.Atoi(digit)
		if err != nil {
			log.Fatalf("failed to convert digit %s to requirement\n", digit)
		}

		m.requirements = append(m.requirements, uint(d))
	}
	assert(len(m.requirements) == int(m.numLights), fmt.Sprintf("req count %d doesn't equal num lights %d\n", len(m.requirements), int(m.numLights)))
}

func readMachine(line string) machine {
	m := machine{}

	// input format
	// [.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
	// now that I know the minimum pushes to get the lights, I can push each button an even number of times to get to the joltage

	fields := strings.Fields(line)
	assert(len(fields) > 0)

	populateLights(&m, fields[0])

	populateButtons(&m, fields[1:len(fields)-1])

	populateRequirements(&m, fields[len(fields)-1])
	return m
}

func toggle(m *machine, lights uint, buttonIdx int) uint {
	return lights ^ m.buttons[buttonIdx]
}

func minButtonPushes(m *machine) int {
	return minButtonPushesRecursive(m, 0, 0)
}

func min(x int, y int) int {
	return -max(-x, -y)
}

func minButtonPushesRecursive(m *machine, current uint, startIdx int) int {
	if current == m.lights {
		return 0
	}

	nButtons := len(m.buttons)
	if startIdx == nButtons {
		return math.MaxInt
	}

	// compare toggling current button with skipping
	return min(1+minButtonPushesRecursive(m, toggle(m, current, startIdx), startIdx+1), minButtonPushesRecursive(m, current, startIdx+1))
}

func joltageEquals(req, current []uint) int {
	if len(req) != len(current) {
		panic("Lengths differ")
	}

	equal := 0
	for i := range req {
		if req[i] < current[i] {
			return -1
		}
		if req[i] > current[i] {
			equal = 1
		}
	}
	return equal
}

func minButtonPushesP2(m *machine) int {
	current := make([]uint, len(m.requirements))
	for i := range current {
		current[i] = 0
	}

	return minButtonPushesRecursiveP2(m, current, 0, 0)
}

func pressButton(m *machine, current []uint, idx int) {
	for i := range 64 {
		if (m.buttons[idx]>>i)&1 == 1 {
			current[i] += 1
		}
	}
}

func unPressButton(m *machine, current []uint, idx int) {
	for i := range 64 {
		if (m.buttons[idx]>>i)&1 == 1 {
			current[i] -= 1
		}
	}
}

func minButtonPushesRecursiveP2(m *machine, current []uint, startIdx int, depth int) int {
	cmp := joltageEquals(m.requirements, current)
	if cmp == 0 {
		return 0
	}
	if cmp == -1 {
		return math.MaxInt
	}

	nButtons := len(m.buttons)
	if startIdx == nButtons {
		return math.MaxInt
	}

	pressButton(m, current, startIdx)
	minForPressing := 1 + minButtonPushesRecursiveP2(m, current, startIdx, depth+1)
	unPressButton(m, current, startIdx)
	minForMoving := minButtonPushesRecursiveP2(m, current, startIdx+1, depth+1)
	fmt.Println(min(minForPressing, minForMoving))

	return min(minForPressing, minForMoving)

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
		var m machine = readMachine(scanner.Text())
		total += minButtonPushes(&m)
	}

	fmt.Printf("Part one: %d\n", total)
}

func p2() {
	f, err := os.Open(INPUT_FILENAME)
	if err != nil {
		log.Fatalf("Could not open file. %s\n", err.Error())
	}

	var total int = 0

	var scanner *bufio.Scanner = bufio.NewScanner(f)
	for scanner.Scan() {
		var m machine = readMachine(scanner.Text())
		total += minButtonPushesP2(&m)
	}

	fmt.Printf("Part two: %d\n", total)
}

func main() {
	p1()
	// p2()
}
