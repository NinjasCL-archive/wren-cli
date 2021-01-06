class Platform {
  foreign static isPosix
  foreign static name

  static isWindows { name == "Windows" }
}

class Process {
  // TODO: This will need to be smarter when wren supports CLI options.
  static arguments { allArguments[2..-1] }
  static env {Env.all}

  foreign static allArguments
  foreign static version
  foreign static cwd
}

class Env {
  // Segmentation fault if name arg is null
  // so a wrapper is needed for public api
  foreign static envGet(name)
  static get(name) {
    if (name is String && name.trim().count > 0) {
      return envGet(name)
    }
    return ""
  }

  foreign static envSet(name)
  static set(name) {
    if (name is String && name.trim().count > 0) {
      return envSet(name)
    }
    return false
  }

  foreign static envRemove(name)
  static remove(name) {
    if (name is String && name.trim().count > 0) {
      return envRemove(name)
    }
    return false
  }

  foreign static envGetAll
  static all {
    var env = {}
    Env.envGetAll.each{|name|
      env[name] = Env.get(name)
    }
    return env
  }
}