# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - 2026-02-28

### Added
- 🎨 Visual editor with grid canvas
- 📦 UI elements palette (Button, Label, Edit, CheckBox, Panel)
- 🔴 Ports on 4 sides (in, out, id, data)
- 🔗 Connections between ports with color coding
- 📋 Element copy function
- ⚙️ JSON compiler/export
- 🖱️ Context menus for elements, ports, and connections
- 📊 Event statistics tracking
- 📄 Properties panel
- ➕ Port management (add/remove ports)
- 🎯 Dashed line preview for connections
- 🗑️ Delete elements, ports, and connections
- 💾 Download project as JSON file

### Technical
- FASM x86 assembly web server
- Win32 API sockets (port 8080)
- HTML5/CSS3/JavaScript frontend
- SVG for connection lines
- Canvas for grid rendering

### Fixed
- Port byte order (htons 8080 = 0x901F)
- Stack cleanup after API calls
- Connection line colors
- Context menu positioning
- Cache control headers

---

## Legend
- `Added` - New features
- `Fixed` - Bug fixes
- `Changed` - Changes in existing functionality
- `Removed` - Removed features
