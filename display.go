package display

import (
	"fmt"
	"os/exec"
)

func StartDisplay(stop chan bool) {
	go func() {
		cmd := exec.Command("./display/IndirectDisplay/x64/Release/IddSampleApp.exe")
		err := cmd.Start()
		if err != nil {
			fmt.Printf("fail to start virtual display %s\n", err.Error())
			return
		}
		<-stop
		err = cmd.Process.Kill()
		if err != nil {
			fmt.Printf("fail to kill virtual display %s\n", err.Error())
			return
		}
	}()
}