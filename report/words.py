from __future__ import annotations
from sys import argv


type Curried[T, U, *As] = tuple[Callable[[*As, T], U], *As]
type Pipe[T, U, *As] = Callable[[T], U] | Curried[T, U, *As]

class Pipeline[T]:
    def __init__(self, val: T) -> None:
        self.val = val

    def __or__[U, *As](self, pipe: Pipe[T, U, *As]) -> Pipeline[U]:
        if isinstance(pipe, tuple):
            fn, *args = pipe
            result = fn(*args, self.val)

        else:
            result = pipe(self.val)

        return Pipeline(result)

    def get(self):
        return self.val


def reduce(op, start, iterable):
    result = start
    for i in iterable:
        result = op(result, i)
    return result


n = 0

def log(thing):
    def put(x):
        print(x, end="")

    global n

    put(thing)
    if n >= 90:
        put("\n")
        n = 0

    else:
        put(" ")
        n += len(thing) + 1

    return thing


def nop(thing):
    return thing


COMMENT_START = "%"
COMMAND_START = "\\"


def main(args: list[str]) -> None:
    process_name = args[0]
    should_log   = False
    found_path   = False
    path         = ""
    
    pos = 1
    while pos < len(args):
        current_arg = args[pos]
        if current_arg in {"-v", "--verbose"}:
            should_log = True
            pos += 1

        elif current_arg in {"-h", "--help"}:
            print(f"{process_name} [Usage]: file.tex [-h|--help] [-v|--verbose]")
            exit(1)

        elif not found_path and not current_arg.startswith("-"):
            path = current_arg
            found_path = True
            pos += 1

        else:
            print(f"{process_name} [Error]: Unknown option {current_arg!r}.")
            exit(1)

    with open(path, "r") as source_file:
        source_text = source_file.read()

    global COMMENT_START, COMMAND_START
    word_count = \
        (Pipeline(source_text)
            # Get a list of lines
            | (lambda source: source.splitlines())
            
            # Strip the lines that are comments out and turn the rest 
            # into a single list of words to be processed futher
            | (map,    lambda line: line.lstrip())
            | (filter, lambda line: not line.startswith(COMMENT_START))
            | (reduce, lambda lines, line: lines + line.split(), [])

            # Strip the latex commands out of our word list
            | (filter, lambda word: not word.startswith(COMMAND_START))

            # Produce the relevant outputs
            | (map, log if should_log else nop)
            | (reduce, lambda count, _: count + 1, 0))

    count = word_count.get()
    print(f"Word count: {count}")


if __name__ == "__main__":
    main(argv)

