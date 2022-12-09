use lazy_static::*;
use std::cell::{Ref, RefCell, RefMut};
use std::env;
use std::io::{stdin, stdout, Write};
use std::path::Path;
use std::process::Command;
use std::str::SplitWhitespace;

pub struct UnSafeCell<T> {
    /// inner data
    inner: RefCell<T>,
}

unsafe impl<T> Sync for UnSafeCell<T> {}

impl<T> UnSafeCell<T> {
    /// User is responsible to guarantee that inner struct is only used in uniprocessor.
    pub unsafe fn new(value: T) -> Self {
        Self {
            inner: RefCell::new(value),
        }
    }
    /// Panic if the data has been borrowed.
    pub fn borrow_mut(&self) -> RefMut<'_, T> {
        self.inner.borrow_mut()
    }

    pub fn borrow(&self) -> Ref<'_, T> {
        self.inner.borrow()
    }
}

const USER: &str = "Clstilmldy";

lazy_static! {
    static ref PATH: UnSafeCell<String> = unsafe {
        UnSafeCell::new(format!("❂ {}   {}\n╰─❯ ", USER, {
            let dir = env::current_dir().unwrap();
            dir.display().to_string()
        }))
    };
}

fn main() {
    loop {
        // Prompt and flush stdout in preparation for stdin
        print!("{}", PATH.borrow());
        stdout().flush().expect("Failed to flush stdout :(");

        // Take in user input
        let mut input = String::new();
        stdin()
            .read_line(&mut input)
            .expect("Failed to read input :(");

        // Split input into command and args
        let mut input = input.trim().split_whitespace(); // Shadows String with SplitWhitespace Iterator
        let cmd = input.next().unwrap();
        match cmd {
            "cd" => cd(input),
            "exit" => break,
            cmd => run_cmd(cmd, input),
        }
    }
}

fn cd(mut args: SplitWhitespace) {
    // The next arg should be the path, if it does not exist then use "/" in it's place
    let target = args.next().unwrap_or("/");
    // Convert &str to Path and then set the current directory to that path (and check for errors)
    let path = Path::new(target);
    if let Err(error) = env::set_current_dir(path) {
        eprintln!("{}", error);
    }

    PATH.borrow_mut().clear();
    PATH.borrow_mut()
        .push_str(&format!("❂ {}   {}\n╰─❯ ", USER, {
            let dir = env::current_dir().unwrap();
            dir.display().to_string()
        }));
}

fn run_cmd(cmd: &str, args: SplitWhitespace) {
    // Spawn command as child-process of the shell
    let child = Command::new(cmd).args(args).spawn();
    // Error checking for child
    match child {
        Ok(mut child) => {
            if let Err(error) = child.wait() {
                eprintln!("{}", error);
            }
        }
        Err(error) => eprintln!("{}", error),
    }
}
