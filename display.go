package display

import (
	"os/exec"
)

func StartDisplay(stop chan bool) {
	go func() {
		cmd := exec.Command("./display/IndirectDisplay/x64/Release/IddSampleApp.exe")
		cmd.Start()
		<-stop
		cmd.Process.Kill()
		cmd.Wait()
	}()
}