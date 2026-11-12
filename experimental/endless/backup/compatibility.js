// SPDX-License-Identifier: GPL-3.0-only
// Audited SMK-37 Pro builds sharing the SMK16P1 bank and B2R1 protocol.
// SS1–SS3: 19619–19621; UI1: 19622; ENDLESS E1–E5: 19623–19627; SP1: 19628; pedal/Panic PED1: 19630; MIDI M1/M1.1: 19631/19632/19633.
export const B2_FIRMWARES=Object.freeze(['19615','19616','19617','19618','19619','19620','19621','19622','19623','19624','19625','19626','19627','19628','19630','19631','19632','19633','19634','19635']);
export const ELITE_FIRMWARES=Object.freeze(['19618','19629','19634']);
export const canRestore=(firmware,model='SMK-37 Pro')=>model==='SMK-37 Elite'?ELITE_FIRMWARES.includes(firmware):model==='SMK-37 Pro'&&B2_FIRMWARES.includes(firmware);
export const supportedFirmware=(firmware,model='SMK-37 Pro')=>(model==='SMK-37 Pro'&&firmware==='19614')||canRestore(firmware,model);
export function supportedIdentity(id){
 const firmware=id?.text?.split('_')[1];
 return ['SMK-37 Pro','SMK-37 Elite'].includes(id?.model)&&supportedFirmware(firmware,id.model)&&id?.text===`${id?.model}_${firmware}`;
}
