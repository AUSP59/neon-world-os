use std::ffi::CStr;
extern "C" {
    fn neon_c_version() -> *const libc::c_char;
}
fn main() {
    unsafe {
        let c = neon_c_version();
        if c.is_null() { println!("unknown"); return; }
        let s = CStr::from_ptr(c).to_string_lossy();
        println!("NEON version (C ABI): {}", s);
    }
}
