use std::{fs::File, io::{self, Read}};
use super::uasm_file::UasmFilePath;

pub fn open_file(uasm_file_path: &UasmFilePath) -> io::Result<File> {
    File::open(uasm_file_path.as_path())
}

pub fn parse_file_to_lines(file: &mut File) -> io::Result< Vec<String> > {
    let mut file_contents = String::new();
    file.read_to_string(&mut file_contents)?;
    Ok(
        file_contents
            .lines()
            .map(|str| str.to_string())
            .collect()
    )
}

pub fn remove_empty_lines(lines: &mut Vec<String>) {
    lines.retain(|str| str != "");
}

pub fn remove_comments(lines: &mut Vec<String>) {
    for line in lines.iter_mut() {
        if let Some(index) = line.find("//") {
            line.truncate(index);
        }
        *line = line.trim_end().trim_start().to_string();
    }

    remove_empty_lines(lines);
}