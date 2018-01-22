module Options = {
  type t = {
    init: bool,
    cache: bool,
    cacheAll: bool,
    ignore: list(string),
    project: string
  };
  let default = {
    init: false,
    cache: false,
    cacheAll: false,
    ignore: [],
    project: "./"
  };
};

let consume = (args, opts, argument) =>
  Options.(
    switch argument {
    | "--init" => {...opts, init: true}
    | "--no-cache" => {...opts, cache: false}
    | "--cache-all" => {...opts, cache: false}
    | "--project" => {...opts, project: args[0]}
    | _ => opts
    }
  );

let parse = args => Array.fold_left(consume(args), Options.default, args);