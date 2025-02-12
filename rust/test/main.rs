use chifir::{*, sys::*};

#[global_allocator]
static ALLOCATOR: chifir::sys::Base::Allocator = chifir::sys::Base::Allocator;

fn main() {
	FatalError!("test");
	Error!("test");
	Warning!("test");
    Info!("test");
	Debug!("test");
	Trace!("test");
	Base::Quit("test");
}
