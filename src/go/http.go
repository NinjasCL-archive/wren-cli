package main

import "C"

import (
	"io"
	"log"
	"net/http"
)

//export Http
func Http() {
	// Set routing rules
	http.HandleFunc("/", Tmp)

	//Use the default DefaultServeMux.
	err := http.ListenAndServe(":8080", nil)
	if err != nil {
			log.Fatal(err)
	}
}

func Tmp(w http.ResponseWriter, r *http.Request) {
	io.WriteString(w, "Calling Go functions from Wren in static libs")
}

func main() {}