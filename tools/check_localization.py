#!/usr/bin/env python3
"""Validate runtime localization contracts using only Python's standard library."""
import plistlib
import re
import struct
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RESOURCE = ROOT / 'Resources'
CATALOG = RESOURCE / 'localization/en.xml'
errors = []

def require(condition, message):
    if not condition:
        errors.append(message)

# plistlib accepts duplicate dictionary keys; reject them explicitly first.
tree = ET.parse(CATALOG)
for dictionary in tree.iter('dict'):
    keys = [node.text for node in dictionary if node.tag == 'key']
    require(len(keys) == len(set(keys)), 'Duplicate XML dictionary key')
data = plistlib.loads(CATALOG.read_bytes())
require(data['language'] == 'en', 'Catalog language must be en')
strings = data['strings']
assets = data['assets']
require('%d' in strings['game.wave_total'] and
        re.findall(r'%(?:\d+\$)?[a-zA-Z]', strings['game.wave_total']) == ['%d'],
        'Wave total must contain exactly one integer placeholder')
for key, value in strings.items():
    require(isinstance(value, str) and bool(value.strip()), f'Empty string: {key}')
    require(not re.search(r'[\u3400-\u9fff]', value), f'Untranslated text: {key}')

for path, entry in assets.items():
    file = RESOURCE / path
    require(file.is_file(), f'Missing artwork (check filename case): {path}')
    if file.is_file():
        header = file.read_bytes()[:24]
        require(header[:8] == b'\x89PNG\r\n\x1a\n', f'Not a PNG: {path}')
        width, height = struct.unpack('>II', header[16:24])
        require((width, height) == (entry['width'], entry['height']),
                f'Artwork size differs from catalog: {path}')
    require(entry['kind'] in ('plate', 'header'), f'Unknown artwork kind: {path}')
    require(entry['font'] > 0, f'Invalid font size: {path}')
    for field in ('key', 'body'):
        if field in entry:
            require(entry[field] in strings, f'Unknown {field} key in {path}')

for file in (ROOT / 'Classes').glob('*.cpp'):
    raw = file.read_bytes()
    try:
        source = raw.decode('utf-8')
    except UnicodeDecodeError:
        source = raw.decode('gb18030')
    # Remove comments before looking for runtime strings/factories.
    source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
    for key in re.findall(r'EnglishUI::text\("([^"\n]+)"\)', source):
        require(key in strings, f'{file.name}: missing runtime string {key}')
    for factory, path in re.findall(
        r'((?:Sprite|MenuItemImage|Button|ImageView)::create)\("([^"\n]+)"', source
    ):
        require(path not in assets,
                f'{file.name}: {factory} bypasses the English artwork for {path}')
    # UI resources must exist with the exact spelling used by first-party callers.
    for path in re.findall(r'"((?:CarrotGuardRes/UI|Carrot/jieshao)/[^"\n]+\.\w+)"', source):
        require((RESOURCE / path).is_file(), f'{file.name}: missing resource {path}')

for file in RESOURCE.rglob('*'):
    if file.suffix in ('.xml', '.tmx', '.tsx', '.plist'):
        ET.parse(file)

if errors:
    print('\n'.join(errors), file=sys.stderr)
    sys.exit(1)
print(f'PASS: {len(strings)} English strings, {len(assets)} localized artwork entries; '
      'XML, runtime references, and resources validated.')
