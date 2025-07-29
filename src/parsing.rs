use std::io;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TokenTag {
    Opcode, //
    Immediate8,
    Address,
    LabelRef,
    Label,
    LabelCreator,
    Register,
    DSetLocation,
    DSetStart,
    DString,
    String
}

#[derive(Debug)]
pub struct Token {
    pub tag: TokenTag,
    pub word: String
}

#[derive(Debug)]
pub struct Label {
    pub address: u16,
    pub name: String
}

pub fn tokenize(lines: &Vec<String>) -> Vec<Token> {
    let mut tokens: Vec<Token> = Vec::new();
    let mut prev_tag: Option<TokenTag> = None;
    let mut labels: Vec<String> = Vec::new();

    for line in lines {
        let mut chars = line.chars().peekable();

        while let Some(&c) = chars.peek() {
            // Skip whitespace
            if c.is_whitespace() {
                chars.next();
                continue;
            }

            // Handle quoted string
            if c == '"' {
                chars.next(); // consume opening quote
                let mut string_content = String::new();

                // Collect everything until closing quote
                while let Some(&next_c) = chars.peek() {
                    chars.next();
                    if next_c == '"' {
                        break;
                    }
                    string_content.push(next_c);
                }

                // Re-add quotes for consistency with your existing string check
                let full_string = format!("\"{}\"", string_content);

                tokens.push(Token {
                    tag: TokenTag::String,
                    word: full_string,
                });

                prev_tag = Some(TokenTag::String);
                continue;
            }

            // Otherwise, collect until next whitespace
            let mut word = String::new();
            while let Some(&next_c) = chars.peek() {
                if next_c.is_whitespace() {
                    break;
                }
                word.push(next_c);
                chars.next();
            }

            // Classify token (same logic as your original)
            let tag: TokenTag = match word.as_str() {
                s if s.starts_with('$') => TokenTag::Immediate8,
                "LBL" => TokenTag::LabelCreator,
                s if matches!(prev_tag, Some(TokenTag::LabelCreator)) => {
                    labels.push(s.to_string());
                    TokenTag::Label
                }
                ".setloc" => TokenTag::DSetLocation,
                ".start" => TokenTag::DSetStart,
                ".string" => TokenTag::DString,
                s if s.starts_with('@') => TokenTag::Address,
                _ if labels.contains(&word) => TokenTag::LabelRef,
                s if s.starts_with('%') => TokenTag::Register,
                s if s.starts_with('"') && s.ends_with('"') => TokenTag::String,
                _ => TokenTag::Opcode,
            };

            tokens.push(Token {
                tag: tag,
                word: word,
            });

            prev_tag = Some(tag);
        }
    }

    tokens
}


pub fn process_tokens(tokens: &Vec<Token>) -> Vec<u8> {
    let mut result: Vec<u8> = vec![0u8; 0x10000];
    let mut ip: u16 = 0x0000;
    let mut iter = tokens.iter().peekable();
    let mut label_table: Vec<Label> = Vec::new();

    while let Some(token) = iter.next() {
        match token.tag {
            TokenTag::Immediate8 => {
                result[ip as usize] = parse_immediate8(&token.word).expect(&format!("INVALID IMMEDIATE VALUE: {}", token.word));
                ip += 1;
                // // println!("Wrote Imm8 0x{:02X}, at 0x{:04X}", result[(ip-1) as usize], ip-1);
            }

            TokenTag::DSetLocation => {
                if let Some(next_token) = iter.next() {
                    match next_token.tag {
                        TokenTag::LabelRef => {
                            ip = label_table
                                .iter()
                                .find(|l| l.name == next_token.word)
                                .map(|l| l.address)
                                .expect(&format!("INVALID LABEL REFERENCE: {}", next_token.word));
                            // println!("Jumped to 0x{:04X}", ip);
                        }

                        TokenTag::Address => {
                            ip = parse_address(&next_token.word)
                                .expect(&format!("INVALID ADDRESS: {}", next_token.word));
                            // println!("Jumped to 0x{:04X}", ip);
                        }

                        _ => {
                            panic!(
                                "Expected an address or label reference after '.setloc', got {:?}",
                                next_token.tag
                            );
                        }
                    }
                } else {
                    panic!("Expected a value after '.setloc', but none found.");
                }
            }

            TokenTag::Address => {
                let addr = parse_address(&token.word).expect(&format!("INVALID ADDRESS: {}", token.word));

                // println!("Wrote Address 0x{:04X}, at 0x{:04X}", addr, ip);

                result[ip as usize] = addr as u8;
                ip += 1;
                result[ip as usize] = (addr >> 8) as u8;
                ip += 1;
            }

            TokenTag::LabelCreator => {
                if let Some(next_token) = iter.next() {
                    // println!("Made a label {} with address 0x{:04X}", next_token.word, ip);
                    label_table.push(Label {
                        address: ip,
                        name: next_token.word.clone()
                    });
                } else {
                    panic!("No Label provided after 'LBL'!");
                }
            }

            TokenTag::DSetStart => {
                if let Some(next_token) = iter.next() {
                    match next_token.tag {
                        TokenTag::LabelRef => {
                            let addr: u16 = label_table
                                .iter()
                                .find(|l| l.name == next_token.word)
                                .map(|l| l.address)
                                .expect(&format!("INVALID LABEL REFERENCE: {}", next_token.word));
                            result[0xfffe] = addr as u8;
                            result[0xffff] = (addr >> 8) as u8;

                            // println!("Set start to 0x{:04X}", addr);
                        }

                        TokenTag::Address => {
                            let addr: u16 = parse_address(&next_token.word)
                                .expect(&format!("INVALID ADDRESS: {}", next_token.word));
                            result[0xfffe] = addr as u8;
                            result[0xffff] = (addr >> 8) as u8;
                            // println!("Set start to 0x{:04X}", addr);
                        }

                        _ => {
                            panic!(
                                "Expected an address or label reference after '.start', got {:?}",
                                next_token.tag
                            );
                        }
                    }
                } else {
                    panic!("Expected a value after '.start', but none found.");
                }
            }

            TokenTag::DString => {
                // Next token should be of type TokenTag::String
                if let Some(next_token) = iter.next() {
                    if next_token.tag == TokenTag::String {
                        // Remove the surrounding quotes `"`
                        let raw = next_token.word.trim_matches('"');

                        // Iterate through each char
                        for ch in raw.chars() {
                            // Write its ASCII byte into memory
                            result[ip as usize] = ch as u8;
                            ip += 1;
                        }
                    } else {
                        panic!("Expected string literal after .string, got {:?}", next_token.tag);
                    }
                } else {
                    panic!("Expected string literal after .string but found nothing");
                }
            },


            TokenTag::Register => {
                result[ip as usize] = match token.word.strip_prefix('%').expect(&format!("Couldn't get register: {}", token.word)) {
                    "A" => 00,
                    "B" => 01,
                    "C" => 02,
                    "D" => 03,
                    "RL" => 04,
                    "RH" => 05,
                    _ => panic!("INVALID REGISTER: {}", token.word)
                };

                // println!("Register {}, wrote 0x{:02X}, at 0x{:04X}", token.word, result[ip as usize], ip);

                ip += 1;
            }

            TokenTag::LabelRef => {
                let addr: u16 = label_table
                                .iter()
                                .find(|l| l.name == token.word)
                                .map(|l| l.address)
                                .expect(&format!("INVALID LABEL REFERENCE: {}", token.word));

                // println!("Wrote Address 0x{:04X}, at 0x{:04X}, in place of label {}", addr, ip, token.word);

                result[ip as usize] = addr as u8;
                ip += 1;
                result[ip as usize] = (addr >> 8) as u8;
                ip += 1;
            }

            TokenTag::Opcode => {
                result[ip as usize] = match handle_opcode(&token.word) {
                    Ok(b) => b,
                    Err(_) => panic!("INVALID OPCODE: {}", token.word)
                };

                // println!("Wrote Byte 0x{:02X} at 0x{:04X}, in place of {}", result[ip as usize], ip, token.word);

                ip += 1;
            }

            _ => {
                panic!("INVALID TOKEN: {:?}", token);
            }
        }
    }

    result
}

pub fn parse_immediate8(word: &str) -> Option<u8> {
    word.strip_prefix('$')
         .and_then(|hex| u8::from_str_radix(hex, 16).ok())
}

pub fn parse_address(word: &str) -> Option<u16> {
    word.strip_prefix('@')
         .and_then(|hex| u16::from_str_radix(hex, 16).ok())
}

pub fn handle_opcode(opcode: &str) -> io::Result<u8> {
    match opcode.to_uppercase().as_str() {
        "NOP" => Ok(0x00),

        "MOVA" => Ok(0x01),
        "MOVB" => Ok(0x02),
        "MOVC" => Ok(0x03),
        "MOVD" => Ok(0x04),
        "CPY"  => Ok(0x05),

        "STR"  => Ok(0x06),
        "LDR"  => Ok(0x07),

        "ADD"  => Ok(0x08),
        "SUB"  => Ok(0x09),
        "INC"  => Ok(0x0A),
        "DEC"  => Ok(0x0B),
        "ADC"  => Ok(0x0C),
        "SBB"  => Ok(0x0D),
        "NEG"  => Ok(0x0E),
        "FLP"  => Ok(0x0F),

        "PSHI" => Ok(0x10),
        "PSHR" => Ok(0x11),
        "POP"  => Ok(0x12),

        "JMP"  => Ok(0x13),
        "CALL" => Ok(0x14),
        "RET"  => Ok(0x15),
        "JZ"   => Ok(0x16),
        "JNZ"  => Ok(0x17),

        "HLT"  => Ok(0x18),

        "JU"   => Ok(0x19),
        "JNU"  => Ok(0x1A),

        "SETU" => Ok(0x1B),
        "CLRU" => Ok(0x1C),
        "SETZ" => Ok(0x1D),
        "CLRZ" => Ok(0x1E),
        "SETN" => Ok(0x1F),
        "CLRN" => Ok(0x20),
        "SETP" => Ok(0x21),
        "CLRP" => Ok(0x22),
        "SETC" => Ok(0x23),
        "CLRC" => Ok(0x24),

        "AND"  => Ok(0x25),
        "OR"   => Ok(0x26),
        "XOR"  => Ok(0x27),
        "NOT"  => Ok(0x28),

        "JE"   => Ok(0x29),
        "JNE"  => Ok(0x2A),
        "JG"   => Ok(0x2B),
        "JL"   => Ok(0x2C),

        "PRNI" => Ok(0x2D),
        "PRNR" => Ok(0x2E),
        "ENPR" => Ok(0x2F),

        "JC"   => Ok(0x30),
        "JNC"  => Ok(0x31),
        "JN"   => Ok(0x32),
        "JNN"  => Ok(0x33),
        "JP"   => Ok(0x34),
        "JNP"  => Ok(0x35),

        "IBPR" => Ok(0x36),
        "LDRO" => Ok(0x37),

        _ => Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            "Invalid opcode",
        )),
    }
}
