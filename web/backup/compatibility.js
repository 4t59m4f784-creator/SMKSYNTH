// SPDX-License-Identifier: GPL-3.0-only
// Audited SMK-37 Pro builds sharing the SMK16P1 bank and B2R1 protocol.
// SS1–SS3: 19619–19621; UI1: 19622; ENDLESS E1–E5: 19623–19627; SP1: 19628.
export const B2_FIRMWARES=Object.freeze(['19615','19616','19617','19618','19619','19620','19621','19622','19623','19624','19625','19626','19627','19628']);
export const canRestore=firmware=>B2_FIRMWARES.includes(firmware);
export const supportedFirmware=firmware=>firmware==='19614'||canRestore(firmware);
export function supportedIdentity(id){
 const firmware=id?.text?.split('_')[1];
 return id?.model==='SMK-37 Pro'&&supportedFirmware(firmware)&&id.text===`SMK-37 Pro_${firmware}`;
}
