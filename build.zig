const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "build.out",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });

    exe.root_module.addIncludePath(b.path("include"));
    exe.root_module.addCSourceFiles(.{
        .files = &.{
            "src/main.c",
            "src/player.c",
            "src/projectile.c",
            "src/enemies.c",
            "src/levelManager.c",
            "src/camera.c",
            "src/audio.c",
            "src/boss.c",
            "src/traps.c",
            "src/ui.c",
            "src/settings.c",
            "src/upgrades.c",
            "src/items.c",
            "src/cJSON.c",
        },
        .flags = &.{
            "-std=gnu23",
            "-O3",
            "-march=native",
            "-D_DEFAULT_SOURCE",
            "-fms-extensions",
            "-Wno-microsoft-anon-tag",
            "-Wall",
            "-Werror",
        },
    });

    const raylib = b.dependency("raylib", .{
        .target = target,
        .optimize = optimize,
    });
    exe.root_module.linkLibrary(raylib.artifact("raylib"));
    b.installArtifact(exe);

    const run_tests = b.addRunArtifact(exe);
    const test_step = b.step("run", "Run");
    test_step.dependOn(&run_tests.step);
}
