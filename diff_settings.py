def apply(config, args):
    config["arch"] = "mipsel"
    config["baseimg"] = "assets/USA/main.exe"
    config["myimg"] = "build/USA/main.exe"
    config["mapfile"] = "build/USA/main.map"
    config["source_directories"] = ["src", "include"]
    config["objdump_executable"] = "mipsel-none-elf-objdump"
    config["map_format"] = "gnu"
    config["build_command"] = ["make", "-j"]
