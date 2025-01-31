use chifir::Info;

#[global_allocator]
static ALLOCATOR: chifir::Base::Allocator = chifir::Base::Allocator;

fn main() {
    Info!("test");
}