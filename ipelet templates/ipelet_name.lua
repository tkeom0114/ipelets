--Template of Lua wrapper for C++ ipelet

--label on ipelet menubar
label = "{ipelet_name}"

--description
about = "{ipelet_description}"

-- this variable will store the C++ ipelet when it has been loaded
ipelet = false

function run(ui)
  if not ipelet then ipelet = assert(ipe.Ipelet(dllname)) end
  ui:runIpelet(label, ipelet) 
end

-- shortcut
shortcuts.ipelet_{ipelet_name}= "Alt+Ctrl+G"
