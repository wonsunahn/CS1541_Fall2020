
var addrSize;
var byteSelectBits;
var wordSelectBits;
var numBlocks;
var assoc;
var isWriteBack;
var bytesPerWord;
var wordsPerBlock;
var numRows;
var rowIndexBits;
var cacheSize;
var tagBits;
var tagStorage;
var validBits;
var dirtyBits;
var validDirtyStorage;
var metadataSize;
var totalStorage;

function update()
{
	addrSize = 32;
	byteSelectBits = 2;
	wordSelectBits = $("#blockSizeSpinner").spinner('value');
	numBlocks = 1 << $("#numBlocksSpinner").spinner('value');

	if($("#assocSpinner").spinner('value') > $("#numBlocksSpinner").spinner('value'))
		$("#assocSpinner").val($("#numBlocksSpinner").val());

	$("#assocSpinner").attr('max', $("#numBlocksSpinner").val());

	assoc = 1 << $("#assocSpinner").spinner('value');
	isWriteBack = $("#writeBack").is(":checked");

	recalculate();
	updateInputDisplay();
	updateStorageDisplay();
	updateAddressDisplay();
	updateDiagram();
}

function recalculate()
{
	bytesPerWord = 1 << byteSelectBits;
	wordsPerBlock = 1 << wordSelectBits;
	numRows = numBlocks / assoc;
	rowIndexBits = Math.log2(numRows);
	cacheSize = bytesPerWord * wordsPerBlock * numBlocks;
	tagBits = addrSize - byteSelectBits - wordSelectBits - rowIndexBits;

	if(tagBits < 0)
	{
		alert("Uh oh, negative tag bits. This shouldn't happen.");
		tagBits = 0;
	}

	tagStorage = tagBits * numBlocks;
	validBits = 1;
	dirtyBits = isWriteBack ? 1 : 0;
	validDirtyStorage = (validBits + dirtyBits) * numBlocks;
	metadataSize = Math.ceil((tagStorage + validDirtyStorage) / 8);
	totalStorage = cacheSize + metadataSize;
}

function updateInputDisplay()
{
	$("span[id^='addrSizeDisplay']").html(addrSize.toString());
	$("span[id^='byteSelectBitsDisplay']").html(byteSelectBits.toString());
	$("span[id^='wordSelectBitsDisplay']").html(wordSelectBits.toString());
	$("span[id^='numBlocksDisplay']").html(numBlocks.toLocaleString());
	$("span[id^='assocDisplay']").html(assoc.toLocaleString());
	$("span[id^='numRowsDisplay']").html(numRows.toLocaleString());
	$("span[id^='rowIndexBitsDisplay']").html(rowIndexBits.toString());
}

function updateStorageDisplay()
{
	$("span[id^='bytesPerWordDisplay']").html(bytesPerWord.toString());
	$("span[id^='wordsPerBlockDisplay']").html(wordsPerBlock.toString());
	$("span[id^='cacheSizeDisplay']").html(cacheSize.toLocaleString());
	$("span[id^='tagBitsDisplay']").html(tagBits.toString());
	$("span[id^='tagStorageDisplay']").html(tagStorage.toLocaleString());
	$("span[id^='validBitsDisplay']").html(validBits.toString());
	$("span[id^='dirtyBitsDisplay']").html(dirtyBits.toString());
	$("span[id^='validDirtyStorageDisplay']").html(validDirtyStorage.toLocaleString());
	$("span[id^='metadataSizeDisplay']").html(metadataSize.toLocaleString());
	$("span[id^='totalStorageDisplay']").html(totalStorage.toLocaleString());
}

function updateAddressDisplay()
{
	$(".addr-display td").show();
	$(".addr-display tr").show();
	if(tagBits == 0) { $(".addr-tag-col").hide(); $("#addrTagRow").hide(); }
	if(rowIndexBits == 0) { $(".addr-row-col").hide(); $("#addrRowRow").hide(); }
	if(wordSelectBits == 0) { $(".addr-word-col").hide(); $("#addrWordRow").hide(); }
	if(byteSelectBits == 0) { $(".addr-byte-col").hide(); $("#addrByteRow").hide(); }
 	$("#addrTagDisplay").html("0".repeat(tagBits));
 	$("#addrRowDisplay").html("0".repeat(rowIndexBits));
 	$("#addrWordDisplay").html("0".repeat(wordSelectBits));
 	$("#addrByteDisplay").html("0".repeat(byteSelectBits));
}

function updateDiagram()
{
	// Single block
	$(".cache-block .addr-tag-cell.bit-display").html("0".repeat(tagBits));
	$(".cache-block td.addr-word-cell").hide();
	$(".cache-block th.addr-word-cell").attr('colspan', wordsPerBlock);

	$(".cache-block td.addr-word-cell").each(function(i, e)
	{
		if(i >= wordsPerBlock)
			return false;

		$(e).show();
	});

	$(".cache-block .dirty-cell").toggle(isWriteBack);

	// Cache layout

	$(".diagram-row-col").show();
	$(".diagram-ellipsis-row").show();
	$(".diagram-row-5-8").show();
	$(".diagram-row-3-4").show();
	$(".diagram-row-2").show();
	$(".diagram-way-row").show();
	$(".diagram-ellipsis-col").show();
	$(".diagram-col-5-8").show();
	$(".diagram-col-3-4").show();
	$(".diagram-col-2").show();

	if(assoc == 1) $(".diagram-way-row").hide();
	if(assoc < 16) $(".diagram-ellipsis-col").hide();
	if(assoc < 8)  $(".diagram-col-5-8").hide();
	if(assoc < 4)  $(".diagram-col-3-4").hide();
	if(assoc < 2)  $(".diagram-col-2").hide();
	$(".diagram-way-header").attr('colspan', Math.min(assoc, 9));

	if(numRows == 1) $(".diagram-row-col").hide();
	if(numRows < 16) $(".diagram-ellipsis-row").hide();
	if(numRows < 8)  $(".diagram-row-5-8").hide();
	if(numRows < 4)  $(".diagram-row-3-4").hide();
	if(numRows < 2)  $(".diagram-row-2").hide();
	$(".diagram-row-header").attr('rowspan', Math.min(numRows, 9));

	/*
	ways column 2: hide if assoc < 2
	ways columns 3-4: hide if assoc < 4
	ways columns 5-8: hide if assoc < 8
	*/

	$(".diagram-way-label").each(function(i, e)
	{
		$(e).html((assoc - 4 + i).toLocaleString());
	});

	$(".diagram-row-label").each(function(i, e)
	{
		$(e).html((numRows - 4 + i).toLocaleString());
	});
}

$(document).ready(function()
{
	$("#blockSizeSpinner").spinner({min: 0, max: 3,  stop: update});
	$("#numBlocksSpinner").spinner({min: 1, max: 16, stop: update});
	$("#assocSpinner").spinner({min: 0, stop: update});
	$("input[type='radio']").checkboxradio();
	$("input[type='radio']").on("change", update);
	// $("fieldset").controlgroup();
	update();
	$("input").change(update);
});