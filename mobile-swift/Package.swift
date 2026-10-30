// swift-tools-version: 5.9
import PackageDescription

let package = Package(
    name: "PyVakitci",
    defaultLocalization: "tr",
    platforms: [
        .iOS(.v15),
        .macOS(.v12)
    ],
    products: [
        .library(
            name: "PyVakitci",
            targets: ["PyVakitci"]
        )
    ],
    targets: [
        .target(
            name: "PyVakitci",
            path: "PyVakitci",
            resources: [
                .process("Resources")
            ]
        )
    ]
)

