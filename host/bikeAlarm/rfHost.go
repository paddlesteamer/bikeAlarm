package main

import (
    "fmt"
    "io/ioutil"
    "log"
    "os/exec"
    "os"
    "strings"
    "time"

    "bikeAlarm/telegram"

    "github.com/jacobsa/go-serial/serial"
)

func  getSerialPort() (string, bool) {
    out, err := exec.Command("/usr/bin/lsusb", "-d", deviceId).Output()

    if err != nil {
        return "", false
    }

    if len(out) == 0 {
        return "", false
    }

    var sysdir = "/sys/bus/usb-serial/devices"
    files, err := ioutil.ReadDir(sysdir)

    if err != nil {
        log.Fatalf("Can't list directory contents of %s", sysdir)
        return "", false
    }

    for _, f := range files {
        if !strings.Contains(f.Name(), "ttyUSB") {
            continue
        }

        ueventf, err := os.Open(fmt.Sprintf("%s/%s/uevent", sysdir, f.Name()))
        if err != nil {
            log.Printf("Can't open %s/%s/uevent", sysdir, f.Name())
            continue
        }

        defer ueventf.Close()

        content, err := ioutil.ReadAll(ueventf)

        if err != nil {
            log.Print("Can't read %s/%s/uevent file", sysdir, f.Name())
            continue
        }

        if strings.Contains(string(content), "DRIVER=pl2303") {
            return fmt.Sprintf("/dev/%s", f.Name()), true
        }
    }

    return "", false
}

func waitForDevice()string {
    for true {
        port, found := getSerialPort()

        if found {
            return port
        }

        time.Sleep(100 * time.Millisecond)
    }

    // should never get here
    return ""
}

func listen(port string) {
    options := serial.OpenOptions{
        PortName : port,
        BaudRate : 9600,
        DataBits : 8,
        StopBits : 1,
        MinimumReadSize: 5,
    }


    dorji, err := serial.Open(options)
    if err != nil {
        log.Fatalf("Can't open connection to %s", port)
        return
    }

    defer dorji.Close()

    buf := make([]byte, 8)
    c := make([]byte, 1)
    msgState := -1
    t := time.Now()
    for true {
        n, err := dorji.Read(c)
        if err != nil {
            break
        }

        if n == 0 {
            continue
        }

        if msgState == -1 {
            if c[0] != '@' {
                continue
            }

            msgState = 0
            t = time.Now()
        } else if c[0] == '!' {
            msgState = -1
            for i:=0; i<len(buf); i++ {
                buf[i] = 0
            }

            if strings.Contains(string(buf), "ALARM") {
                telegram.SendMessage("BikeAlarm")
                fmt.Println("ALARM")
            }
        } else {
            cur := time.Now()

            //if cur.Sub(t).Milliseconds() > 2000 {
            if cur.Sub(t) > 2000000000 { // because of old golang on rpi
                msgState = -1
                for i:=0; i<len(buf); i++ {
                    buf[i] = 0
                }
                continue
            }

            buf[msgState] = c[0]

            msgState++
        }

    }
}

func RFHost() {

    telegram.Init(telegramToken, telegramChatId)

    for true {
        fmt.Printf("Waiting for device\n")
        port := waitForDevice()

        fmt.Printf("Device found. Listening...\n")
        listen(port)
    }

}
