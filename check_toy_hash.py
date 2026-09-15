#!/usr/bin/env python3

import argparse
from pathlib import Path

UINT64_MASK = 0xffffffffffffffff


def toy_hash(data: bytes) -> int:
    h = 0xcbf29ce484222325
    for byte in data:
        h ^= byte
        h = (h * 0x100000001b3) & UINT64_MASK
    h ^= h >> 33
    h = (h * 0xff51afd7ed558ccd) & UINT64_MASK
    h ^= h >> 33
    h = (h * 0xc4ceb9fe1a85ec53) & UINT64_MASK
    h ^= h >> 33
    return h & 0x0000ffffffffffff

def insert_at(big: bytes, pos: int, small: bytes) -> bytes:
    as_array = bytearray(big)
    for i, b in enumerate(small):
        as_array[pos + i] = b
    return bytes(as_array)

def number_to_hexbyte(num: int) -> str:
    as_hex = "0123456789abcdef"

    result = ""
    while num:
        result = as_hex[num & 0xf] + result
        num >>= 4

    return result


def main() -> None:
    parser = argparse.ArgumentParser(description="Compute the toy_hash of a file.")
    parser.add_argument("file", type=Path)
    parser.add_argument("nonce", type=int)
    args = parser.parse_args()

    file = args.file.read_bytes()
    nonce = number_to_hexbyte(args.nonce).encode("utf-8")
    file_with_nonce = insert_at(file, 16, nonce)
    file_with_id = insert_at(file_with_nonce, 33, "23821639".encode("utf-8"))

    value = toy_hash(file_with_id)
    print(f"{value:012x}")


if __name__ == "__main__":
    main()
