--Lua wrapper for C++ ipelet

--label on ipelet menubar
label = "Inscribed triangle"

--description
about = "Ipelet for finding maximal axis-aligned triangle which inscribed in a simple polygon"

-- this variable will store the C++ ipelet when it has been loaded
ipelet = false

function run(ui)
  if not ipelet then ipelet = assert(ipe.Ipelet(dllname)) end
  ui:runIpelet(label, ipelet) 
end

-- shortcut
shortcuts.ipelet_inscribed_triangle= "Alt+Ctrl+I"
