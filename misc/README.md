# Misc nanoservices tools

## ns-configurator

Nanoservices configuration tool. As nanoservices may be configured in several different ways, 
and configuration strategy is chosen at the installation playground, it is highly recommended 
to use this tool for configuration purposes, as it allows for transparent configuration across 
all the supported configuration systems.

Usage:

```
ns-configurator {-c|--create|-u|--update} {-p|--path} /path/to/config/node {-a|--data} DATA
ns-configurator {-r|--read|-d|--delete} {-p|--path} /path/to/config/node
ns-configurator {-h|--help}
```

**-c**, **--create** - create a configuration node at the given path containing DATA

**-u**, **--update** - update a configuration node at the given path with DATA. Previous node value is deleted

**-r**, **--read** - read a configuration node at the given path, show contained data as JSON in STDOUT

**-d**, **--delete** - delete a configuration node at the given path

**-h**, **--help** - show help in STDOUT

Examples:

`ns-configurator -c /my/data/path -a 'MYDATA'` - will create a node `/my/data/path` and set its value to `MYDATA`

`ns-configurator -u /my/data/path -a '{"foo":"bar", "echo": "lool"}'` - will update the node at path `/my/data/path`
with two subnodes, `foo` and `echo`, and set their values according to the given JSON object



## ns-shutdown

Tool for properly service shutdown
