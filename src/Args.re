module Options = {
  type t = {
    init: bool,
    cache: bool,
    cacheAll: bool,
    project: string,
    ignore: list(string),
    build: list(string)
  };
  let default = {
    init: false,
    cache: false,
    cacheAll: false,
    project: "./",
    ignore: [],
    build: []
  };
};

let rec find_next_argument_index = (args, argument, index) =>
  if (args[index] === argument) {
    index + 1;
  } else {
    find_next_argument_index(args, argument, index + 1);
  };

let consume = (args, opts, argument) =>
  Options.(
    switch argument {
    | "--init" => {...opts, init: true}
    | "--no-cache" => {...opts, cache: false}
    | "--cache-all" => {...opts, cache: false}
    | "--project" =>
      let projectPathIndex = find_next_argument_index(args, "--project", 0);
      {...opts, project: args[projectPathIndex]};
    | "--build" =>
      let buildIndex = find_next_argument_index(args, "--build", 0);
      let argsAfterIndex =
        Array.sub(args, buildIndex, Array.length(args) - buildIndex);
      {...opts, build: Array.to_list(argsAfterIndex)};
    | "--ignore" =>
      let ignoreIndex = find_next_argument_index(args, "--ignore", 0);
      let argsAfterIndex =
        Array.sub(args, ignoreIndex, Array.length(args) - ignoreIndex);
      Js.log(argsAfterIndex);
      {...opts, ignore: Array.to_list(argsAfterIndex)};
    | _ => opts
    }
  );

let parse = args => Array.fold_left(consume(args), Options.default, args);