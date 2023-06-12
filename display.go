package display

import (
	"os/exec"
)

func StartDisplay() {
	go exec.Command("./display/IndirectDisplay/x64/Release/IddSampleApp.exe").Output()
}