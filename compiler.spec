# -*- mode: python ; coding: utf-8 -*-

from PyInstaller.utils.hooks import collect_data_files
import os

block_cipher = None

# 添加pyd文件到datas
def get_pyd_files():
    pyd_files = []
    pyd_dir = os.path.join('build', 'lib', 'Release')
    
    # 需要包含的pyd文件
    pyd_files_to_include = [
        'AST.pyd',
        'TAC.pyd', 
        'lexical.pyd',
        'syntactic.pyd'
    ]
    
    for pyd_file in pyd_files_to_include:
        src_path = os.path.join(pyd_dir, pyd_file)
        if os.path.exists(src_path):
            pyd_files.append((src_path, '.'))
    
    return pyd_files

a = Analysis(
    ['main.py'],
    pathex=[],
    binaries=[],
    datas=get_pyd_files() + [
        ('icon.ico', '.'),
        ('main_ui.ui', '.'),
        ('main_ui_src.qrc', '.'),
        ('convert_ast.py', '.'),
        ('icon.py', '.'),
        ('main_ui.py', '.')
    ],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

# 添加PySide6相关文件
pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.zipfiles,
    a.datas,
    [],
    name='compiler',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    icon='icon.ico'
)

# 可选：如果需要打包成单文件，取消下面的注释
# exe = EXE(
#     pyz,
#     a.scripts,
#     [],
#     exclude_binaries=True,
#     name='compiler',
#     debug=False,
#     bootloader_ignore_signals=False,
#     strip=False,
#     upx=True,
#     runtime_tmpdir=None,
#     console=False,
#     disable_windowed_traceback=False,
#     argv_emulation=False,
#     target_arch=None,
#     codesign_identity=None,
#     entitlements_file=None,
#     icon='icon.ico'
# )