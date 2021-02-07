const { app, BrowserWindow } = require("electron");

const createWindow = () => {
	const win = new BrowserWindow({
		width: 1280,
		height: 720,
		webPreferences: {
			contextIsolation: false,
			nodeIntegration: true
		}
	});

	win.setMenu(null);
	win.webContents.openDevTools();
	win.loadFile("index.html");
};

app.whenReady().then(createWindow);

app.on("window-all-closed", () => {
	if(process.platform !== "darwin") {
		app.quit();
	}
});

app.on("activate", () => {
	if(BrowserWindow.getAllWindows().length === 0) {
		createWindow();
	}
}); 

