use std::{env, fs::File, io::{self, Write}};
mod uasm_file;
mod file_handling;
mod parsing;
use file_handling::*;
use uasm_file::UasmFilePath;
use parsing::*;

fn main() -> io::Result<()> {

    let filename: String = env::args().nth(1).unwrap_or_else(|| {
        eprintln!("Usage: program <file.uasm>");
        std::process::exit(1);
    });

    let uasm_file_path = UasmFilePath::new(&filename).unwrap_or_else(|e| {
        eprintln!("{}, pass a UASM file next time!", e);
        std::process::exit(2);
    });

    let mut file = open_file(&uasm_file_path).unwrap_or_else(|e| {
        eprintln!("Couldn't open file!\nReason: {}", e);
        std::process::exit(3)
    });

    let mut lines = parse_file_to_lines(&mut file)?;
    remove_comments(&mut lines);
    let tokens = tokenize(&lines);
    let rom = process_tokens(&tokens);

    let mut out_file: File = File::create("out.bin").expect("Couldn't create the output file!");

    out_file.write_all(&rom)?;
    
    Ok(())
}
