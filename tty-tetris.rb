class TtyTetris < Formula
 desc "Terminal-based Tetris game"
 homepage "https://github.com/raulgotor/tty-tetris"
 url "https://github.com/raulgotor/tty-tetris/archive/v0.1.0.tar.gz" # Point to the tarball of the tag
   version "0.1.0"  # Include the version
 license "MIT"

  depends_on "cmake" => :build
  depends_on "ncurses"  # If you use ncurses for terminal display

  def install
    system "cmake", "-S", ".", "-B", "build", *std_cmake_args
    system "cmake", "--build", "build"
    system "cmake", "--install", "build"
  end

  test do
    system "#{bin}/tty-tetris", "--version"  # Assuming you have a --version flag
  end
end