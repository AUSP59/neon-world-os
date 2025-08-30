class NeonWorldOs < Formula
  desc "Host-embedded OS simulator (C++)"
  homepage "https://example.invalid/neon-world-os"
  url "https://github.com/your/repo/releases/download/v210.0.0/neon-world-os-210.0.0.tar.gz"
  sha256 "REPLACE_ME"
  license "MIT"

  depends_on "cmake" => :build

  def install
    system "cmake", "-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"
    system "cmake", "--build", "build", "-j"
    system "cmake", "--install", "build", "--prefix", prefix
  end

  test do
    system "#{bin}/neon", "--version"
  end
end
