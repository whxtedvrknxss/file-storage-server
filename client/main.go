package main

import (
	"fmt"
	"io"
	"net/http"
	"sync"
	"time"
)

const (
	serverURL  = "http://127.0.0.1:8080/files/test_download.txt"
	numClients = 1000
	logBuffer  = 1000
)

type LogMsg struct {
	Time time.Time
	Msg  string
}

func logger(logChan <-chan LogMsg, done chan<- struct{}) {
	for entry := range logChan {
		fmt.Printf("[%s] %s\n",
			entry.Time.Format("15:04:05.000"),
			entry.Msg,
		)
	}
	done <- struct{}{}
}

func runClient(id int, wg *sync.WaitGroup, logChan chan<- LogMsg) {
	defer wg.Done()

	transport := &http.Transport{
		DisableKeepAlives: true,
		ForceAttemptHTTP2: false,
	}

	client := &http.Client{
		Transport: transport,
		Timeout:   0,
	}

	logChan <- LogMsg{
		Time: time.Now(),
		Msg:  fmt.Sprintf("[Client %02d] Sending request...", id),
	}

	startTime := time.Now()

	req, err := http.NewRequest("GET", serverURL, nil)
	if err != nil {
		logChan <- LogMsg{
			Time: time.Now(),
			Msg:  fmt.Sprintf("[Client %02d] Error creating request: %v", id, err),
		}
		return
	}

	resp, err := client.Do(req)
	if err != nil {
		logChan <- LogMsg{
			Time: time.Now(),
			Msg:  fmt.Sprintf("[Client %02d] Request failed: %v", id, err),
		}
		return
	}
	defer resp.Body.Close()

	bytesRead, err := io.Copy(io.Discard, resp.Body)
	if err != nil {
		logChan <- LogMsg{
			Time: time.Now(),
			Msg:  fmt.Sprintf("[Client %02d] Error reading response: %v", id, err),
		}
		return
	}

	duration := time.Since(startTime)

	logChan <- LogMsg{
		Time: time.Now(),
		Msg: fmt.Sprintf(
			"[Client %02d] HTTP %d (%d bytes) in %v",
			id, resp.StatusCode, bytesRead, duration,
		),
	}
}

func main() {
	fmt.Printf("Starting NO-keepalive test with %d clients...\n", numClients)

	var wg sync.WaitGroup

	logChan := make(chan LogMsg, logBuffer)
	done := make(chan struct{})

	go logger(logChan, done)

	globalStart := time.Now()

	for i := 1; i <= numClients; i++ {
		wg.Add(1)
		go runClient(i, &wg, logChan)

		time.Sleep(100 * time.Microsecond)
	}

	wg.Wait()

	close(logChan)
	<-done

	fmt.Printf("\nAll downloads finished in %v!\n", time.Since(globalStart))
}
