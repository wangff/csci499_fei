load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_library(
    name = "mergesort",
    srcs = ["mergesort.cpp"],
    hdrs = ["mergesort.h"],
    visibility = ["//main:__pkg__"]
)

cc_binary(
    name = "test",
    srcs = ["test.cpp"],
    deps = [
        ":mergesort",
    ],
)