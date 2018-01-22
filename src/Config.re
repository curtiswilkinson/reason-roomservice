type hook =
  | Single(option(string))
  | List(option(list(string)));

type room = {
  path: option(string),
  before: hook,
  runParallel: hook,
  runSynchronous: hook,
  after: hook,
  finally: hook
};

type config = {rooms: list(room)};

let is_file: string => bool = [%raw
  {|
  function(path) {
    return Fs.lstatSync(path).isFile()
  }
|}
];

let rec first_matching_config = files =>
  switch files {
  | [] =>
    Js.log("No files in dir");
    exit(1);
  | ["roomservice.config.yml"] => "roomservice.config.yml"
  | ["roomservice.config.toml"] => "roomservice.config.toml"
  | ["roomservice.config.json"] => "roomservice.config.json"
  | [_, ...xs] => first_matching_config(xs)
  };

let parse = (path, contents) => {
  open Node;
  let ext = Path.parse(path)##ext;
  switch ext {
  | ".yml" => Js.log("YAML")
  | ".toml" => Js.log("toml")
  | ".json" => Js.log("JSON")
  | _ => Js.log("unknown config format")
  };
  Js.log(contents);
};

let get = projectPath => {
  open Node;
  let isFile = is_file(projectPath);
  let path =
    if (isFile) {
      projectPath;
    } else {
      Fs.readdirSync(projectPath) |> Array.to_list |> first_matching_config;
    };
  let contents = Fs.readFileAsUtf8Sync(path);
  parse(path, contents);
};