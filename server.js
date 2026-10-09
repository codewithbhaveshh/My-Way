const express = require('express');
const { execFile } = require('child_process');
const path = require('path');
const fs = require('fs');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.static(path.join(__dirname, 'public')));

app.get('/api/route', (req, res) => {
    const startLat = req.query.startLat || req.query.start_lat;
    const startLon = req.query.startLon || req.query.start_lon || req.query.startLng || req.query.start_lng;
    const endLat = req.query.endLat || req.query.end_lat;
    const endLon = req.query.endLon || req.query.end_lon || req.query.endLng || req.query.end_lng;

    if (!startLat || !startLon || !endLat || !endLon) {
        return res.status(400).json({
            error: "startLat, startLon, endLat, and endLon query parameters are required"
        });
    }

    let enginePath = path.join(__dirname, 'core', 'engine.exe');
    if (!fs.existsSync(enginePath)) {
        enginePath = path.join(__dirname, 'core', 'router.exe');
    }
    if (!fs.existsSync(enginePath)) {
        enginePath = path.join(__dirname, 'router.exe');
    }

    execFile(enginePath, [String(startLat), String(startLon), String(endLat), String(endLon)], { cwd: __dirname }, (error, stdout, stderr) => {
        if (error) {
            return res.status(500).json({ error: stderr || error.message || "Routing engine execution failed" });
        }

        try {
            const result = JSON.parse(stdout);
            res.json(result);
        } catch (e) {
            res.send(stdout);
        }
    });
});

app.listen(PORT, () => console.log(`🚀 NaviNCR Middleware running on http://localhost:${PORT}`));