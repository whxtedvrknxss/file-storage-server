package main

import (
	"fmt"
	"io"
	"net/http"
	"sync"
	"time"
)

const (
	// Point this to a valid test file path in your server's sandbox
	serverURL  = "http://127.0.0.1:8080/files/test_download.txt"
	numClients = 10000
)

func runClient(id int, wg *sync.WaitGroup) {
	defer wg.Done()

	// Build the GET request
	req, err := http.NewRequest("GET", serverURL, nil)
	if err != nil {
		fmt.Printf("[Client %02d] Error creating request: %v\n", id, err)
		return
	}

	// Custom client configuration with an execution timeout limit
	client := &http.Client{
		Timeout: 10 * time.Second,
	}

	fmt.Printf("[Client %02d] Sending concurrent GET request...\n", id)
	startTime := time.Now()

	resp, err := client.Do(req)
	if err != nil {
		fmt.Printf("[Client %02d] Catastrophic failure: %v\n", id, err)
		return
	}
	defer resp.Body.Close()

	// Crucial for File Servers: Read the actual file bytes into the bit-bucket (io.Discard)
	// This forces your C++ server to fully stream the file across the socket wire.
	bytesRead, err := io.Copy(io.Discard, resp.Body)
	if err != nil {
		fmt.Printf("[Client %02d] Error reading stream payload: %v\n", id, err)
		return
	}

	duration := time.Since(startTime)
	fmt.Printf("[Client %02d] Received HTTP %d (%d bytes) in %v\n", id, resp.StatusCode, bytesRead,
		duration)
}

func main() {
	fmt.Printf("Starting asynchronous GET test with %d concurrent clients...\n", numClients)
	globalStart := time.Now()

	var wg sync.WaitGroup

	// Launch 20 concurrent clients asynchronously using goroutines
	for i := 1; i <= numClients; i++ {
		wg.Add(1)
		go runClient(i, &wg)
	}

	// Wait for all 20 loops to complete
	wg.Wait()

	fmt.Printf("\nAll concurrent downloads finished execution in %v!\n", time.Since(globalStart))
}
