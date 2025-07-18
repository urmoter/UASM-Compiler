use std::path::{Path, PathBuf};
use std::io;
use std::fmt;

impl fmt::Display for UasmFilePath {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.0.display())
    }
}

pub struct UasmFilePath(PathBuf);
impl UasmFilePath {
    pub fn new<T: AsRef<str>>(path: T) -> Result<Self, io::Error> {
        let path_ref = path.as_ref();
        if !path_ref.to_lowercase().ends_with(".uasm") {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                "Not a .uasm file",
            ));
        }
        
        Ok(Self(PathBuf::from(path_ref)))
    }

    pub fn as_path(&self) -> &Path {
        &self.0
    }
}