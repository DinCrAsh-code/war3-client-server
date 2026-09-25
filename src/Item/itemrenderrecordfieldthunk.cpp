//============================================================================
//  0x6F2F4940 - `mov ecx,[ecx+0x24C]` / `jmp sub_6F37D620`, two instructions.
//  An adjustor: shift `this` to the sub-object at +0x24C, then tail-jump
//  into a real, unhooked, otherwise-unreconstructed shipped method
//  (0x6F37D620, out of this batch) on it.  Reached from
//  SItemRemoveHelpers::FileRenderRecord (0x6F333330, batch 3's own
//  territory) - CItem-adjacent, hence Item/ - but the callee itself is not
//  a reconstruction of anything this batch can see, so THUNK, the same
//  reasoning ability_typedep.cpp's own GetFallbackThreshold gives for the
//  identical `mov ecx,[ecx+N]` / `jmp <target>` shape.
//============================================================================

//  0x6F37D620's own real `retn 4` (verified against its own raw bytes -
//  `pop esi / pop ebp / pop ebx / pop ecx / retn 4`) means this adjustor
//  cleans four bytes too, one stack argument beyond the `this` it shifts
//  through ecx - `tools/thunk_abi_audit.py` catches a plain `(void*)`
//  declaration here as a live crash (this build's own arity disagreeing
//  with the callee's real `retn`), which is what the second, otherwise
//  unused parameter below is for: to get the arity right, not because
//  anything in this call tree gives it a name or a real type.
//
//  0x6F37D620 has no reconstruction anywhere in this tree to name as a real
//  extern - an undefined `extern "C"` reference to it would compile and
//  score fine but leaves the real link (`tools/link_check.py`) with an
//  unresolved external, since nothing on the real link line defines a bare
//  `sub_<addr>` symbol either.  The literal address is the only thing that
//  actually resolves, the same `mov eax,<addr>` an ADDR_THUNK macro would
//  use - it costs this thunk one extra instruction against the dump's own
//  two, which does not matter for a THUNK's score.
__declspec(naked) void __fastcall ForwardToFieldAt0x24C(void*, float)
{
    __asm { mov ecx, [ecx+24Ch] }
    __asm { mov eax, 0x6F37D620 }
    __asm { jmp eax }
}
