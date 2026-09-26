# Function summaries: main (0x00401050)

- Binary: 04_MiniRtsExample.exe - x86, 32-bit Windows PE EXE, image base 0x00400000, entry point 0x00419CD1
- Call graph: `C:\Users\New\Desktop\projects\RE\ingame\my_tools\just_another_re_pipeline\extractor\output\04_MiniRtsExample.exe\cfg_output.json` - 241 functions
- 232 functions summarized by the LLM (each exactly once), 125 callsite analyses, 9 functions needed no LLM
- Recursion: 1 self-recursive functions, 1 recursive cycles
- Root context given: This is the program's main().
- 3 warnings - see the end of this report

## Call tree

Each function is expanded the first time it appears; later appearances say _see above_.

- `0x00401050` **main** - Serves as the application's entry point (`main`), hashing command-line arguments to build a configuration seed, initializing core runtime subsystems like sin...
  - `0x00417ED0` **InitSineTable** - Initializes a precomputed sine or trigonometric lookup table containing 1024 float values.
    - `0x0041A20C` **_CIsin** - Import thunk: jumps straight to the imported API MSVCR80!_CIsin.
  - `0x004189C0` **InitCrc32Table** - Initializes the standard CRC32 lookup table using the polynomial 0xEDB88320.
  - `0x0041A150` **memset** - Import thunk: jumps straight to the imported API MSVCR80!memset.
  - `0x004012B0` **ResourceBlock_Initialize** - Initializes a large resource block data structure (approx 0x5BBE4 bytes) containing various internal lists, states, and an array of 7 elements via vector con...
    - `0x00401690` **ResourceObject_Destructor** _(address taken)_ - Destroys or unlinks an individual RM-header resource object, updating global tracking counters and either freeing its memory or returning it to a custom free...
    - `0x00401680` **ElementDestructor_12Bytes** _(address taken)_ - Destroys or clears a 12-byte object/array element by zeroing out its first 12 bytes.
    - `0x004199C2` **__vec_ctor** - Constructs an array of objects using a provided constructor function, handling exception unwinding via `__ArrayUnwind` if any constructor throws an exception.
      - `0x00419DE8` **__SEH_prolog4** - Sets up a standard Microsoft Visual C++ Structured Exception Handling (SEH) stack frame with security cookie checks and variable local frame allocation.
        - `0x00419E41` **__except_handler4** _(address taken)_ - Standard MSVC SEH exception handler shim that forwards exception records to _except_handler4_common along with the security cookie and checker function.
          - `0x004198F2` **__security_check_cookie** _(address taken)_ - Validates that the stack canary value passed in ecx matches the global security cookie, jumping to the GS failure handler if a mismatch is detected.
            - `0x00419CDB` **___report_gsfailure** _(tail jump)_ - Standard MSVC buffer security check (GS) failure handler that saves processor and stack context, notifies the CRT debugger, invokes unhandled exception filte...
              - `0x0041A11A` **_crt_debugger_hook** - Import thunk: jumps straight to the imported API MSVCR80!_crt_debugger_hook.
          - `0x0041A120` **_except_handler4_common** - Import thunk: jumps straight to the imported API MSVCR80!_except_handler4_common.
      - `0x00419E2D` **__SEH_epilog4** - Tears down a standard MSVC Structured Exception Handling (SEH) frame, restoring the previous exception handler chain, stack pointer, and saved registers befo...
      - `0x00419901` **__ArrayUnwind** - Iterates backwards through an array of objects to invoke their destructor function, protected by a Structured Exception Handling (SEH) filter that calls term...
        - `0x00419DE8` **__SEH_prolog4** _(see above)_
        - `0x00419DE0` **?terminate@@YAXXZ** - Import thunk: jumps straight to the imported API MSVCR80!?terminate@@YAXXZ.
        - `0x00419E2D` **__SEH_epilog4** _(see above)_
    - `0x004198F2` **__security_check_cookie** _(see above)_
    - `0x0041A224` **__CxxFrameHandler3** _(tail jump)_ - Import thunk: jumps straight to the imported API MSVCR80!__CxxFrameHandler3.
  - `0x00401750` **InitializeSimulationAndResourceSubsystem** - Initializes the primary resource block, game state subsystems, spatial grids, PRNG states, and entity registration tables before running initial simulation s...
    - `0x00418760` **InitializePrngState** - Initializes a state structure with a pseudo-random number generator (PRNG) state based on a seed, employing hash mixing constants and repeated mixing rounds.
    - `0x004186E0` **InitializeSubManager** - Initializes a sub-manager structure by allocating an 8000-byte block and setting up a linked list or table of 4096 (0x1000) entries with zeroed values and in...
      - `0x004185D0` **AllocateCustomBlock** - Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc fo...
        - `0x004184E0` **InitializeSubsystem** - Initializes the custom allocation subsystem and its associated global tracking structures once upon the first allocation request.
        - `0x00418430` **FindAllocationBinIndex** - Performs a binary search over a predefined table of size thresholds (`word_41B150`) to find the appropriate pool or bin index for a given allocation size.
        - `0x00418470` **AllocateMemoryPoolBlock** - Allocates a large 64KB memory chunk via malloc, initializes a linked list of fixed-size sub-blocks within it for a specific pool bucket, and registers the ne...
        - `0x0041A150` **memset** _(see above)_
    - `0x0041A150` **memset** _(see above)_
    - `0x00401F60` **InitializeSubsystemRecords** - Initializes an array of structured configuration records within a large resource block using a loop, followed by setting up two additional specific subsystem...
      - `0x00404150` **InitializeRecord** - Initializes a structured configuration record in a state buffer by setting field bytes, zeroing blocks with memset, computing a bit shift value, and copying...
        - `0x0041A150` **memset** _(see above)_
    - `0x00416640` **UpdateResourceStateBlockOrSimulationStep** - Performs a comprehensive simulation, procedural generation, spatial grid update, and entity/particle batch processing step using a large state structure.
      - `0x00415220` **GenerateNoiseHeightmap** - Generates a 2D grid/map of procedural noise values by evaluating noise grid functions across a double loop.
        - `0x00415180` **EvaluateProceduralNoiseGrid** - Iterates a specified number of times, accumulating values computed by a procedural noise/hash function (`ComputeNoiseOrRandomCellValues`) combined with float...
          - `0x00414FA0` **ComputeNoiseOrRandomCellValues** - Performs complex floating-point calculations combined with a pseudo-random integer mixing hash function (resembling a value noise or hash-based procedural ge...
            - `0x0041A160` **__ftol2_sse** - Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.
      - `0x004152B0` **ProcessSpatialGridOrCollisionData** - Performs intensive nested-loop floating-point calculations, coordinate distance/intersection evaluations, and square root operations over a large grid or mul...
        - `0x0041A212` **_CIsqrt** - Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.
      - `0x004153F0` **ComputeDistanceFieldOrGridCollision** - Iterates through a multi-dimensional grid structure (up to 9417 elements in nested loops) performing geometric distance calculations and collision/intersecti...
        - `0x00414AF0` **CalculateGridPointerOrMetric** - Performs geometric bounding/clipping checks and coordinate-to-grid index calculations using x87 floating-point math, returning a computed pointer or metric v...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0041A212` **_CIsqrt** _(see above)_
      - `0x00415D40` **UpdateParticleSystemOrEmitter** - Updates particle generator state and buffers using extensive floating-point calculations, coordinate transformations, and pseudo-random number generation.
        - `0x0041A160` **__ftol2_sse** _(see above)_
      - `0x004156B0` **UpdateParticleBatchState** - Performs an inner loop batch update over a multi-dimensional array of particle or entity state elements, applying floating-point math, bounds clamping, and i...
        - `0x0041A160` **__ftol2_sse** _(see above)_
      - `0x00415770` **UpdateGridCellStates** - Iterates over a 2D grid of elements (from index 2 to 93/0x5E), performing neighbor comparisons, distance/geometric checks, and updating cell state flags.
      - `0x004159E0` **ProcessSpatialGridBatchOrParticleUpdate** - Iterates through a nested 2D grid/cell structure to perform geometric metric calculations, evaluate procedural noise, check thresholds, and update state reco...
        - `0x0041A212` **_CIsqrt** _(see above)_
        - `0x00415180` **EvaluateProceduralNoiseGrid** _(see above)_
        - `0x00418050` **CalculateSpatialOrPathMetric** - Performs complex floating-point geometry, clamping, and polynomial evaluation calculations using values on the x87 FPU stack.
        - `0x00418140` **ClampFloat** - Performs a clamp operation on a floating-point value between a lower and upper bound using x87 FPU instructions.
        - `0x004188F0` **UpdateRngAndCompute** - Updates a 64-bit pseudo-random number generator state (or hash/state structure) passed via `esi`, performs bitwise mixing and arithmetic operations, and exec...
      - `0x0041A160` **__ftol2_sse** _(see above)_
      - `0x00414E40` **CheckGridAreaOrFindNearby** - Validates a 2D grid cell or searches outward in concentric square rings up to a given radius for a valid/accessible grid position, returning the first matchi...
    - `0x00418470` **AllocateMemoryPoolBlock** _(see above)_
    - `0x00410A90` **ResourceEntry_Initialize** - Initializes a newly allocated resource or sub-system entry object passed in `ecx` using an identifier/lookup value (`Src`) passed in `eax`.
    - `0x004185D0` **AllocateCustomBlock** _(see above)_
    - `0x0041A22A` **memcpy** - Import thunk: jumps straight to the imported API MSVCR80!memcpy.
    - `0x00402050` **SpawnResourceOrEntityBatch** - Processes a specific resource or state entry by calculating spatial metrics, normalizing vectors, validating grid positions, and spawning/registering entity...
      - `0x00417FD0` **Vec3_NormalizeOrZero** - Computes the Euclidean length of a 3D vector and normalizes it, returning 1/length or 0 if the length is zero or near-zero.
        - `0x0041A212` **_CIsqrt** _(see above)_
      - `0x00418050` **CalculateSpatialOrPathMetric** _(see above)_
      - `0x00418140` **ClampFloat** _(see above)_
      - `0x00402560` **CreateAndRegisterResourceEntity** - Allocates, initializes, and configures a new resource or RTS object entity based on type lookup tables, coordinates, and flags, registering it into spatial g...
        - `0x004185D0` **AllocateCustomBlock** _(see above)_
        - `0x00410720` **RtsObject_InitSpecializedComponent_410720** - Initializes a specialized RTS game object or component structure by setting up its virtual method table, initializing base sub-objects, and configuring sever...
          - `0x0040B100` **RtsObject_InitializeBase** - Initializes a newly allocated resource or sub-element structure by setting up its virtual table, clearing its state fields, and iterating through a lookup ta...
            - `0x00411580` **RtsComponentFactory_Create** - Factory function that allocates and initializes various RTS component or sub-component objects based on a type index lookup table.
              - `0x004185D0` **AllocateCustomBlock** _(see above)_
              - `0x00413870` **RtsSubComponentA_Init** - Initializes a specific sub-component object instance (case A of the factory dispatcher) using configuration parameters and a lookup table.
              - `0x00414150` **CComponentType2Factory_Constructor** - Initializes a specific component object (case 2 factory type) using configuration tables and a provided parameter index.
              - `0x004117B0` **RtsComponentType3_Constructor** - Initializes a newly allocated component object (factory case 3) by setting up its virtual table, storing configuration parameters, clamping and scaling an in...
              - `0x004119C0` **RtsComponentCase4_Init** - Initializes a specific RTS component object (case 4 in the factory dispatcher) using an index, a byte parameter, and a configuration table look-up.
              - `0x00411CB0` **FactoryCase5Constructor** - Initializes a sub-component object (case 5 in the factory dispatcher) by setting up a virtual table, storing configuration indexes and pointers, and initiali...
              - `0x00411DD0` **RtsComponentType6_Init** - Initializes a subtype 6 component object instance using a lookup table (`byte_41BDC0`) and index parameters.
              - `0x004121B0` **RtsComponent7_Constructor** - Initializes a newly allocated component object (case 7 in the factory dispatcher) using an ID index, a configuration byte argument, and a lookup table in rea...
              - `0x00412330` **RtsComponentType8_Constructor** - Initializes a newly allocated component object of type 8, setting up its virtual method table, configuration index, and lookup-derived flags.
              - `0x004124B0` **SubComponentType9_Constructor** - Initializes an object instance (allocated at `eax`) for factory case 9, setting up its virtual method table, boundary-checked configuration values, and refer...
              - `0x00412950` **RtsComponent10_Constructor** - Initializes a specific RTS game component structure (factory case 10) by setting up its virtual method table, storing initialization parameters, and configur...
              - `0x00412C80` **RtsComponentType11_Init** - Initializes a specific RTS component object (factory case 11) using configuration data from a lookup table and storing various parameters.
              - `0x00412D40` **RtsSubComponent12_Constructor** - Initializes an object instance (case 12 sub-component) by setting up virtual method tables, storing configuration parameters, and calculating lookup properti...
              - `0x00412E50` **FactoryItem13_Constructor** - Initializes a newly allocated component object (factory case 13) using parameters passed in registers and on the stack, setting up virtual table pointers and...
              - `0x00413050` **InitializeComponentType14** - Initializes a newly allocated component object (factory case 14) by setting up its virtual method table, initializing fields based on the provided index and...
              - `0x00413390` **RtsSubComponent15_Constructor** - Initializes a specific sub-component object (case 15 factory instance) by setting up virtual tables, member fields, and bounds-checked lookup references.
              - `0x004137C0` **ComponentTypeInfo_Initialize** - Initializes a component descriptor/configuration object (`this` in `eax`) using a lookup table (`byte_41BDC0`) indexed by a validated `type_index` (`ecx`), a...
            - `0x004198F2` **__security_check_cookie** _(see above)_
            - `0x0041A224` **__CxxFrameHandler3** _(tail jump, see above)_
        - `0x0040E760` **RtsObject_InitializeDerivedType** - Initializes a specific derived RTS object instance, setting up its virtual table, searching a static lookup table to configure its type properties, and initi...
          - `0x0040B100` **RtsObject_InitializeBase** _(see above)_
        - `0x0040F880` **RtsObject_InitializeSubVariant** - Initializes a specialized resource sub-element variant by calling the base initializer and configuring specific floating-point and scalar state fields.
          - `0x0040B100` **RtsObject_InitializeBase** _(see above)_
        - `0x0040B100` **RtsObject_InitializeBase** _(see above)_
        - `0x00418730` **AllocateResourceIndexOrNode** - Performs a bounds check on an array/pool structure, and if valid, allocates an element by updating indices, copying handles, and incrementing an allocation c...
        - `0x00414AA0` **ConvertAndClampCoordinate** - Scales a floating-point coordinate value, converts it to an integer, and clamps the result to the range [0, 95].
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x00414DC0` **UpdateResourceGridFlags** - Iterates over a 2D coordinate range (grid/map sub-structure) to set or clear specific flag bits within a data table.
        - `0x0040B790` **ConfigureResourceBlock** - Initializes, computes dimensions and properties for, and configures a newly created resource block (passed in ebx) using lookup tables, resource property loo...
          - `0x00404460` **LookupResourceProperty** - Searches a static configuration table in read-only data against properties of a given resource structure to compute or retrieve a floating-point resource par...
          - `0x004043B0` **EvaluateResourceProperty** - Evaluates a resource block's configuration data and lookup tables to compute a scaling factor or dimension, returning the resulting floating-point value.
        - `0x00404100` **GrowContainerCapacity** - Grows the capacity and reallocates the underlying data buffer of a container structure when it reaches capacity.
          - `0x00418670` **ReallocateCustomBlock** - Reallocates a custom tracked memory block, growing or shrinking it while preserving existing data, or handles in-place resizing if the current block's pool b...
            - `0x004185D0` **AllocateCustomBlock** _(tail jump, see above)_
            - `0x0041A22A` **memcpy** _(see above)_
            - `0x00401000` **FreeResourceBlock** - Frees a custom memory block with an 'RM' header, updating global tracking statistics and either returning it to an internal free list or releasing it via fre...
        - `0x004175E0` **InsertIntoSpatialGrid** - Calculates the grid cell index for an entity using its 2D coordinates and inserts it into a linked list at that cell position in a spatial hash grid.
          - `0x00417410` **CalculateGridIndex** - Calculates a combined integer grid or cell index from 2D floating-point coordinates by scaling, rounding via __ftol2_sse, clamping to bounds [0, 23], and com...
            - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x004042F0` **RegisterResourceEntry** - Increments usage counters and updates statistical or positional fields for a resource sub-element based on lookup criteria and flags.
        - `0x00401000` **FreeResourceBlock** _(tail jump, see above)_
        - `0x004198F2` **__security_check_cookie** _(see above)_
        - `0x0041A224` **__CxxFrameHandler3** _(tail jump, see above)_
      - `0x0041A160` **__ftol2_sse** _(see above)_
      - `0x00414CE0` **CheckGridAreaPassesCriteria** - Tests whether a 2D rectangular grid region within a structured data array meets specific validity and value criteria, returning true if all checked elements...
      - `0x00414E40` **CheckGridAreaOrFindNearby** _(see above)_
    - `0x0041A160` **__ftol2_sse** _(see above)_
    - `0x00414CE0` **CheckGridAreaPassesCriteria** _(see above)_
    - `0x00402560` **CreateAndRegisterResourceEntity** _(see above)_
    - `0x00407B50` **ProcessRtsResourceSpawningOrPathNodes** - Iterates through waypoint or resource spawn definitions, evaluates spatial metrics and procedural selection checks via geometric calculations and pseudo-rand...
      - `0x0041A212` **_CIsqrt** _(see above)_
      - `0x00418050` **CalculateSpatialOrPathMetric** _(see above)_
      - `0x0041A160` **__ftol2_sse** _(see above)_
      - `0x00402560` **CreateAndRegisterResourceEntity** _(see above)_
    - `0x00402980` **EntityInteractionEventCallback** _(address taken)_ - Serves as a callback wrapper that validates an object context and state before forwarding an event to the entity interaction handler.
      - `0x004084C0` **HandleEntityInteractionEvent** - Validates an entity or resource index from an object context, optionally resolves a handle, and conditionally invokes an entity interaction processor.
        - `0x004029F0` **LookupResourceOrHandle** - Resolves a 32-bit handle (comprising an index and a validation tag) into a managed object pointer, verifying bounds, a tag match, and an internal status byte.
        - `0x004080A0` **ProcessEntityInteractionContext** _(tail jump)_ - Iterates through a list of entity references in a context structure, validates each entity against compatibility and state rules, and triggers an entity inte...
          - `0x0040CAF0` **ValidateEntityPairCompatibility** - Performs a multi-stage validation check between two entities (passed in ebx and esi) to determine if they are compatible for interaction or spatial association.
            - `0x0040BD40` **ValidateEntitySubComponent** - Validates specific bit flags and a byte state field within an object structure to determine sub-component compatibility.
            - `0x0040BCD0` **sub_40BCD0** - { "suggested_name": "ValidateEntityCompatibility", "prototype": "bool __fastcall ValidateEntityCompatibility(void *contextObject, void *entityReference)", "s...
          - `0x0040DAC0` **ProcessEntityInteractionOrCommand** - Validates, resolves, and processes an entity interaction or command based on state flags and type constraints, optionally queueing or executing state transit...
            - `0x0040D900` **ResolveEntityInteractionType** - Determines the interaction type code between two entities and populates a result buffer starting with the computed code.
              - `0x0040CAF0` **ValidateEntityPairCompatibility** _(see above)_
            - `0x0040DA90` **IsValidEntityType** - Validates whether a given entity type code falls within an accepted set using a switch/jump table mechanism.
            - `0x0040BED0` **FindResourceEntryByParam** - Searches an array of pointers within a resource object at offset 0x210 to find an entry matching a specific property value (`edi`), returning the matching po...
            - `0x00413930` **CleanupSecondaryEntityOrResource** - Cleans up a secondary entity or referenced resource associated with an object, decrementing a reference count if conditions are met and resetting state flags.
              - `0x004029F0` **LookupResourceOrHandle** _(see above)_
            - `0x0040BF70` **ProcessEntitySubItems** - Iterates over an array of sub-objects/items contained within an entity if a specific flag is set, invoking a virtual method on each item that meets criteria.
            - `0x0040C100` **ResetEntityStateFields** - Resets several internal state and property fields within an entity structure to zero.
    - `0x00404570` **InitializeEntityRegistration** - Initializes a new entity/resource structure and registers it into a global tracking array or proximity-matching container.
      - `0x00404640` **CheckProximityMatch** _(address taken)_ - Validates entity state and computes spatial distance against a threshold to update registration or trigger status flags.
        - `0x00417F80` **CalculateDistance** - Computes the Euclidean distance between two 2D points (x1, y1) and (x2, y2).
          - `0x0041A212` **_CIsqrt** _(see above)_
      - `0x004198B0` **ResizeContainer** - Resizes the underlying data array of a dynamic container structure, doubling or scaling capacity until it can satisfy a required minimum size hint.
        - `0x00418670` **ReallocateCustomBlock** _(see above)_
    - `0x004097E0` **InitializeStateBlock** - Initializes a state or configuration block object via `this` (passed in `ecx`), setting up vtable/type pointers, zeroing fields, and populating internal conf...
    - `0x0040A0D0` **VM_ExecuteEntityEventScript** - Validates and executes a virtual machine bytecode block associated with a specific entity event, managing VM state backup and restoration around the execution.
      - `0x00409940` **VM_ExecuteBytecodeBlock** _(recursive)_ - Executes a block of virtual machine bytecode instructions within a given context, managing operand stacks, local variables, flow control, and dispatching bui...
        - `0x004098B0` **VM_PushValue** - Pushes a 64-bit value onto the virtual machine's internal stack, checking for overflow and setting an error flag if the stack limit (256 entries) is exceeded.
        - `0x004098F0` **VM_PopOperand** - Pops an 8-byte value (variant/operand) from the virtual machine's internal stack structure, returning false if the stack is underflowed or true on success.
        - `0x004094B0` **ExecuteBinaryOp** - Performs binary arithmetic operations (add, subtract, multiply, divide, modulo) on two script/bytecode variable operands, handling both integer and floating-...
          - `0x0041A21E` **_CIfmod** - Import thunk: jumps straight to the imported API MSVCR80!_CIfmod.
        - `0x00409680` **CompareVariantValues** - Compares two variant/operand buffers (left in ECX, right in EAX) using a relational operator specified by EDX, supporting both integer and floating-point com...
        - `0x00409480` **CheckOperandTruthState** - Evaluates an operand structure's truth state or validity flag, returning true or false based on its type and value.
        - `0x0040A2A0` **OpcodeHandler_LoadConstantOrVariable** _(vtable entry)_ - Executes a virtual machine instruction handler that retrieves a value (possibly a constant or variable) and writes an 8-byte typed value structure to the out...
        - `0x0040A2D0` **OpcodeRandomRange** _(vtable entry)_ - Executes a bytecode range/random instruction by extracting low and high bounds (converting floats to ints if needed), generating a random integer within that...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040A380` **ExecuteRandomRangeOp** _(vtable entry)_ - Virtual opcode handler that evaluates operands (possibly as integer bounds or floating-point values), queries a pseudo-random number generator, and writes ou...
          - `0x004188F0` **UpdateRngAndCompute** _(see above)_
        - `0x0040A3F0` **ExecuteSpawnEntityScriptOpcode** _(vtable entry)_ - Executes a bytecode script instruction that spawns and registers a game entity based on coordinates, type, and variant parameters extracted from the script o...
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x00414AA0` **ConvertAndClampCoordinate** _(see above)_
          - `0x00414E40` **CheckGridAreaOrFindNearby** _(see above)_
          - `0x00402560` **CreateAndRegisterResourceEntity** _(see above)_
        - `0x0040A540` **ResolveScriptReference** _(vtable entry)_ - Validates and dereferences a script/bytecode resource identifier, performing bounds checking against a global table and invoking a virtual method on the reso...
        - `0x0040A5A0` **VM_GetGameVariableFloat** _(vtable entry)_ - Resolves a bytecode variable operand to a floating-point value from a global game state table, returning 0.0f if the reference is invalid.
        - `0x0040A620` **ExecuteScriptOpcode_TypeCheckOrRangeOp** _(vtable entry)_ - Executes a specialized bytecode operation or validation handler from a script dispatch vtable, validating an operand reference and performing floating-point...
        - `0x0040A6E0` **BytecodeOpcodeHandler_Slot7** _(vtable entry)_ - Executes a specific virtual bytecode instruction handler that dereferences an operand structure, checks its validity, and populates a status/result output st...
        - `0x0040A720` **BytecodeOpcode_Slot8_EvalOrDefault** _(vtable entry)_ - Evaluates a nested script/bytecode operand structure or retrieves a default value, writing a type tag and a value pointer into an output status structure.
        - `0x0040A760` **VM_ResolveResourceHandle** _(vtable entry)_ - Validates and resolves a resource/object handle from bytecode operand data, returning a status code and index/handle value.
        - `0x0040A7E0` **ExecuteBytecodeOp_10** _(vtable entry)_ - Executes virtual opcode slot 10, performing type checks/conversions on operands (using __ftol2_sse) and updating state variables stored in a global data stru...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040A830` **VM_OpcodeHandler_Slot11** _(vtable entry)_ - Executes a virtual machine bytecode instruction handler that processes typed operands (converting floating-point to integer if necessary) and updates a globa...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040A880` **ExecuteBytecodeTypeOp** _(vtable entry)_ - Executes a virtual machine bytecode opcode handler that checks a type-tagged operand, extracts or converts its value, performs a lookup in a global table, an...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040A8E0` **ExecuteBytecodeOpcode13** _(vtable entry)_ - Executes a specific bytecode instruction handler (vtable slot 13) that validates a resource ID and type, writing status and resulting data values back to the...
        - `0x0040A960` **EvalScriptReference** _(vtable entry)_ - Evaluates a script or bytecode reference operand (checking type tag 3, index bounds, and validity) to produce a floating-point value written to an output buf...
        - `0x0040A9E0` **ScriptOp_GetResourceValue** _(vtable entry)_ - Evaluates a resource/object identifier operand from bytecode, bounds-checking it against a global resource manager table, and returns a floating-point or typ...
        - `0x0040AA60` **ExecuteEntityInteractionOrCommandOpcode** _(vtable entry)_ - Executes a specialized entity interaction or command bytecode instruction, validating an entity reference and marshaling arguments into a parameter structure...
          - `0x0040DAC0` **ProcessEntityInteractionOrCommand** _(see above)_
        - `0x0040AB80` **ExecuteBytecodeVtableSlot17** _(vtable entry)_ - Executes a virtual opcode handler that evaluates a type-checked operand (converting floats to integers or reading integer fields), computes a table lookup in...
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x004044D0` **LookupBytecodeTable** - Iterates through a hardcoded table in the data section, conditionally accumulating 16-bit field values from a provided structure based on bit flags.
        - `0x0040ABE0` **HandleBytecodeStatusOrEvent** _(vtable entry)_ - Extracts an integer value or type code from a bytecode operand structure and uses it as an index to trigger event handlers if it falls within a valid range (...
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x00403BF0` **TriggerHandlersByIndex** - Performs a squared-index lookup into a global data structure to flag a status bit and iterates through an array of registered objects, invoking a virtual met...
        - `0x0040AC10` **ExecuteOpcode19** _(vtable entry)_ - Implements a bytecode virtual machine instruction handler (vtable slot 19) that checks type and value flags against a global constraint table.
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040AC70` **ExecuteScriptMathOrConversionOp** _(vtable entry)_ - Executes a virtual script opcode that performs math, type conversion, or variable creation on input script values, returning a newly allocated script variabl...
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x004027A0` **CreateScriptVariable** - Allocates and initializes a new script variable object of a specified type, registers it within a global runtime container pool, and returns a pointer to the...
            - `0x004185D0` **AllocateCustomBlock** _(see above)_
            - `0x00410BD0` **ScriptVariable_Initialize** - Initializes a script variable or runtime property object with default values, a vtable pointer, and a type-specific property computed from a passed-in identi...
            - `0x00404100` **GrowContainerCapacity** _(see above)_
        - `0x0040AD00` **ScheduleTimedEventOrState** _(vtable entry)_ - Evaluates a timed/parameterized script value, checks against scheduling limits, and enqueues a new state or event entry via EnqueueStateEntry.
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x00419280` **EnqueueStateEntry** - Allocates or inserts a new element into a state/priority queue container, resizes the container if capacity is exceeded, populates the new entry with provide...
            - `0x0040A1E0` **DispatchEntityEvent** _(address taken)_ - Checks if an index is within bounds and an element flag is active, then extracts a byte identifier from the element and dispatches it to sub_40A0D0.
              - `0x0040A0D0` **VM_ExecuteEntityEventScript** _(calls back up a recursive cycle)_
            - `0x00419800` **ResizeStateCapacity** - Grows the capacity and reallocates the underlying data buffer of a state/allocation context object to accommodate a new required size.
              - `0x00418670` **ReallocateCustomBlock** _(see above)_
            - `0x00419590` **sub_419590** - { "suggested_name": "HeapUp", "prototype": "void __fastcall HeapUp(int element_index, void *collection_structure)", "short_summary": "Performs a 'heap-up' (s...
        - `0x0040ADF0` **VM_OpcodeHandler_Slot22** _(vtable entry)_ - Executes a virtual machine opcode handler (vtable slot 22) that reads an operand, converts or extracts an integer value, checks bounds against an array limit...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040AE80` **ExecuteScriptOpcode_Slot23** _(vtable entry)_ - Executes a virtual opcode handler (vtable slot 23) in the bytecode interpreter, fetching and converting operand data based on type flags to perform a math or...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040AEF0` **ExecuteVmOpcodeSlot24** _(vtable entry)_ - Executes a virtual machine opcode handler (vtable slot 24) that processes operand types, converts values using floating-point operations or type checks, and...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x0040AF60` **BytecodeOpcode_HandleReferenceOrResource** _(vtable entry)_ - Resolves a packed resource/reference identifier from a bytecode operand, validates it against a global manager table and object state, invokes a virtual meth...
        - `0x0040AFE0` **VerifyObjectReference** _(vtable entry)_ - Validates a structured object/resource reference from bytecode operands against a global registry table and returns a status flag.
        - `0x0040B060` **VirtualOpcodeHandler_Slot27** _(vtable entry)_ - Executes a specific script/bytecode virtual opcode method from vtable slot 27, writing a status code and an integer value to an output structure.
        - `0x0040B090` **GetSystemStateOrValue** _(vtable entry)_ - Retrieves a global state value and writes a 64-bit output structure containing a boolean flag and the retrieved value.
        - `0x0040B0C0` **HandleOpcodeType2** _(vtable entry)_ - Handles a specific bytecode/script operand or instruction by checking its type tag and storing either a converted floating-point or direct integer value into...
          - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x00409460` **GetFloatOperand** - Extracts and converts a floating-point operand value based on its type tag, returning it as a double in st(0).
        - `0x0041A160` **__ftol2_sse** _(see above)_
    - `0x004036B0` **UpdateSimulationGridRegions** - Iterates through sub-components and grid regions in the simulation resource structure, clearing flags and conditionally invoking grid region updates.
      - `0x00417B50` **ProcessGridRegionAction** - Performs coordinate scaling, clamped grid indexing, and a loop-driven region update on a 2D grid/terrain structure using floating-point math and trigonometri...
        - `0x0041A160` **__ftol2_sse** _(see above)_
        - `0x00417A70` **UpdateGridCellRegion** - Performs bounds-checked modifications and lookups on a 2D grid/terrain array (96x96 space) based on floating-point coordinate transformations and loop parame...
          - `0x0041A160` **__ftol2_sse** _(see above)_
  - `0x00403E50` **RtsManager::RunSimulationLoopAndFinalize** - Runs the RTS simulation ticks until a completion condition is met, flushes and compresses the network/resource buffer, performs checksum and verification pas...
    - `0x00403D30` **RtsManager::UpdateSimulationTick** - Executes a single simulation tick for the RTS manager, incrementing counters, processing network stream batches, updating entities, grids, and priority queue...
      - `0x00408C10` **RtsManager::ProcessNetworkStreamBatches** - Iterates through serialized packet or command batches from a network stream buffer, deserializes object states, and dispatches command batches to the RTS man...
        - `0x00408A00` **DeserializeObjectState** - Deserializes serialized object network or snapshot state from a bitstream into a structure, validating length and header constraints.
          - `0x00418D10` **StreamReadBits** - Reads a variable number of bits (up to 32) from a bitstream context, advancing the read position and handling end-of-stream bounds.
          - `0x00418D80` **BitStream_ReadVariableLengthValue** - Reads a variable-length bit-packed integer value from a bit-stream state structure passed in ebx.
        - `0x00408D70` **RtsManager::ProcessCommandOrActionBatch** - Iterates over a batch of entity commands or resource requests packed within a structure, validates them against game state and entity references, and dispatc...
          - `0x0040BED0` **FindResourceEntryByParam** _(see above)_
          - `0x0040F3D0` **RtsManager::UpgradeComponent** - Validates and advances a specific component level within an RTS manager object, optionally instantiating or updating the underlying component instance via th...
            - `0x00411580` **RtsComponentFactory_Create** _(see above)_
          - `0x00408CE0` **CalculateGridCoordinatesOrMapping** - Calculates coordinate mapping or grid indices using an integer square root approximation loop followed by floating-point arithmetic operations.
          - `0x0040DAC0` **ProcessEntityInteractionOrCommand** _(see above)_
      - `0x00419300` **ProcessPriorityQueueTick** - Extracts elements from a binary heap / priority queue, invokes a callback function on them under certain conditions, and re-inserts or manages elements in a...
        - `0x004185D0` **AllocateCustomBlock** _(see above)_
        - `0x0041A22A` **memcpy** _(see above)_
        - `0x00419690` **HeapSiftDown** - Performs a binary heap sift-down (or heapify-down) operation on a collection passed in edi, restoring the heap property by moving elements down.
        - `0x00418670` **ReallocateCustomBlock** _(see above)_
        - `0x00419590` **sub_419590** _(see above)_
      - `0x004033A0` **CheckSimulationBoundsOrState** - Evaluates floating-point conditions and bounds, and conditionally sets flags on an array of sub-objects stored within a large resource structure.
      - `0x00403460` **CManager::UpdateEntitiesOrUnits** - Iterates through an array of entity/unit structures within a manager object, conditionally performing update ticks and invoking a sequence of combat, AI, abi...
        - `0x00404940` **UpdateEntitySelectionAndLists** - Iterates through a global list of game units/entities, filtering and categorizing them into various sub-lists and tracking counts within the specified entity...
        - `0x00404AD0` **Entity::ProcessCombatOrAbilityUpdate** - Evaluates combat proximity and ability conditions for an entity, dispatching targeted command packets or triggering unit abilities and actions based on dista...
          - `0x00402E50` **FindNearestValidEntityInRadius** - Iterates through a global list of entities, filtering by flags, health, and a squared distance radius, and returns a pointer to the closest matching entity (...
          - `0x0041A212` **_CIsqrt** _(see above)_
          - `0x004046B0` **DispatchCommandPacket** - Constructs and serializes an outgoing network command packet using a local bitstream buffer, checking global packet-rate limits before transmission.
            - `0x00418AA0` **BitStream_WriteBits** - Writes a specified number of bits from an integer value into a bitstream buffer, updating bit offsets and tracking overflow status.
            - `0x004087D0` **SerializeCommandPacket** - Serializes a complex command or action packet structure into a bitstream using bit-packing, varints, and normalized floats.
              - `0x00418AA0` **BitStream_WriteBits** _(see above)_
              - `0x00418B10` **WriteBitStreamVarInt** - Serializes an integer value as a variable-length bit-packed integer (varint-style encoding using 7-bit chunks with continuation bits) into a bitstream buffer.
              - `0x00418C80` **BitStream_WriteNormalizedFloat** - Serializes and writes a floating-point value clamped to a specified range into a bitstream as a quantized fixed-point integer.
                - `0x00418AA0` **BitStream_WriteBits** _(see above)_
          - `0x00402A30` **FindEntityInRange** - Iterates through a global list of entities, checks their active state, distance to a specified 2D coordinate, and grid/pathfinding constraints, returning the...
          - `0x00405110` **CheckAndExecuteUnitAbility** - Validates resource cooldowns, unit requirements, and target constraints for a specific ability/action on an entity, and dispatches a command packet if all ch...
            - `0x00404260` **ValidateCapacityOrLimit** - Validates whether a calculated value based on an entity's fields and an argument is within a capped limit (0x64), returning a boolean gating further execution.
            - `0x004046B0` **DispatchCommandPacket** _(see above)_
          - `0x00404DF0` **CheckEntityConditionsOrCount** - Queries entity state and loops through a list of sub-elements to calculate an accumulated count or conditional metric, combined with a timing/distance check...
          - `0x00405300` **Entity::TryPerformActionOrCommand** - Validates timing cooldowns, resource/structure criteria, geometric placement, and target availability for an entity action, and if successful, dispatches a c...
            - `0x00405520` **CheckEntityPlacementOrActionCriteria** - Evaluates complex geometric, proximity, and grid placement/action criteria for an entity, checking surrounding entities and map grid regions before validatin...
              - `0x00402E50` **FindNearestValidEntityInRadius** _(see above)_
              - `0x0041A212` **_CIsqrt** _(see above)_
              - `0x00417FD0` **Vec3_NormalizeOrZero** _(see above)_
              - `0x0041A160` **__ftol2_sse** _(see above)_
              - `0x00414CE0` **CheckGridAreaPassesCriteria** _(see above)_
              - `0x004182F0` **IntersectRaySegmentOrLine** - Performs a 2D geometric intersection or closest-point/projection calculation (likely between a ray/line segment and a point or other geometric primitive), po...
            - `0x004053D0` **Entity::FindTargetOrValidSubEntity** - Iterates through an entity's sub-components or list of associated elements, performs distance and state validation checks, and returns the first matching ele...
              - `0x0041A212` **_CIsqrt** _(see above)_
            - `0x004046B0` **DispatchCommandPacket** _(see above)_
        - `0x00404D20` **Entity::ProcessAIOrProductionAction** - Evaluates entity-specific production or action progress and criteria, and potentially triggers a command via Entity::TryPerformActionOrCommand.
          - `0x00405300` **Entity::TryPerformActionOrCommand** _(see above)_
        - `0x00405040` **Entity::ProcessTimedBehaviorsOrActions** - Iterates through a configuration-driven sequence of state actions or behavior frames for an entity, using Entity::ProcessStateTransitionOrAction, and updates...
          - `0x00404EA0` **Entity::ProcessStateTransitionOrAction** - Evaluates a configuration command/action byte array against entity state attributes and dispatches corresponding actions or updates state flags.
            - `0x00404DF0` **CheckEntityConditionsOrCount** _(see above)_
            - `0x00405300` **Entity::TryPerformActionOrCommand** _(see above)_
            - `0x00405520` **CheckEntityPlacementOrActionCriteria** _(see above)_
            - `0x00417EB0` **GetConfigTableEntry** - Retrieves a pointer to a configuration structure or entry from a static table in .rdata based on an input index.
            - `0x00404260` **ValidateCapacityOrLimit** _(see above)_
            - `0x00405110` **CheckAndExecuteUnitAbility** _(see above)_
            - `0x00405230` **CheckAndDispatchAction** - Validates command cooldown timers, resource requirements, and target state for a specific action index, returning false if conditions fail or dispatching a c...
              - `0x004046B0` **DispatchCommandPacket** _(see above)_
        - `0x004079D0` **CUnit::EvaluateAbilitiesAndDispatchCommands** - Iterates over a collection of sub-components or queued capabilities within an entity, validating timer thresholds, target/combat conditions, and rate limits...
          - `0x0041A160` **__ftol2_sse** _(see above)_
          - `0x004046B0` **DispatchCommandPacket** _(see above)_
          - `0x00406EF0` **Entity_CheckAndSerializeCommand** - Validates entity state/timers and serializes a network command packet into a bitstream if rate limits and conditions are satisfied.
            - `0x00418AA0` **BitStream_WriteBits** _(see above)_
            - `0x004087D0` **SerializeCommandPacket** _(see above)_
          - `0x004070A0` **CUnit::EvaluateCombatActionOrAcquireTarget** - Evaluates unit combat states, checks proximity/range to targets or entities in an internal array, and dispatches command packets if criteria are met.
            - `0x004046B0` **DispatchCommandPacket** _(see above)_
            - `0x004030C0` **FindNearestValidEntityInRange** - Iterates through a global list of entities/objects, filtering out inactive, dead, or invalid items and finding one that satisfies specific state flags and li...
          - `0x004071C0` **EvaluateEntityAbilityCriteriaAndCast** - Evaluates spatial queries and targeting criteria for an entity's sub-components, checking conditions such as counts of nearby filtered entities or raycast li...
            - `0x0040C950` **ValidateEntityCriteriaFilter** _(address taken)_ - Evaluates whether a given entity matches a comprehensive set of filtering criteria, flags, type constraints, and spatial or status conditions specified in a...
              - `0x004042B0` **CheckEntityTypeMatch** - Evaluates whether a given entity matches a target entity type or bitmask category, returning true if it matches and false otherwise.
              - `0x00403320` **CheckEntityCriteriaMatch** - Evaluates a specific filter criterion on an entity by inspecting its status flags, type attributes, and either checking a bitfield or falling back to a grid...
                - `0x00417D70` **EvaluateGridLookup** - Performs a 2D grid or lookup-table index calculation using floating-point coordinates and bounds clamping, then checks a bit within a data structure.
                  - `0x0041A160` **__ftol2_sse** _(see above)_
            - `0x004177F0` **SpatialGridQueryEntities** - Queries a spatial grid structure for entities within a bounding box or radius, filters them using distance checks and an optional callback, and populates an...
              - `0x004174B0` **CalculateGridBounds** - Calculates and clamps spatial grid or bounding box indices from floating-point coordinate ranges, storing the resulting integer indices into output pointers.
                - `0x0041A160` **__ftol2_sse** _(see above)_
            - `0x00417F80` **CalculateDistance** _(see above)_
            - `0x004046B0` **DispatchCommandPacket** _(see above)_
            - `0x00417FD0` **Vec3_NormalizeOrZero** _(see above)_
            - `0x004183C0` **CheckRaySegmentCollisionOrDistance** - Performs a 2D geometric test by calling a ray/segment intersection routine and evaluating vector dot products or squared distances to determine a boolean out...
              - `0x004182F0` **IntersectRaySegmentOrLine** _(see above)_
        - `0x00406760` **CUnit::UpdateAIStateOrBehavior** - Evaluates and updates an entity's high-level state machine or behavior mode (stored at offset +0x0C) by checking environmental conditions, proximity values,...
          - `0x00405AD0` **CalculateEntityStatsOrAttributes** - Performs aggregate mathematical computations on collections of attributes or modifier pointers stored within an entity structure, iterating over arrays in ch...
          - `0x004062E0` **CalculateEntityCentroidOrAverage** - Calculates the average spatial position or centroid from an entity's internal collections of points and referenced objects, storing the result in an output s...
          - `0x00417FD0` **Vec3_NormalizeOrZero** _(see above)_
          - `0x00406540` **FindNearestValidEntityOrTarget** - Scans entity lists and grid structures to find a valid target or entity meeting specific distance and filtering criteria relative to the given entity.
            - `0x00417E10` **CheckEntityGridFilter** - Converts spatial coordinates into grid/lookup indices via floating-point arithmetic and integer clamping, then checks a bit flag within a lookup table or gri...
              - `0x0041A160` **__ftol2_sse** _(see above)_
            - `0x00417F80` **CalculateDistance** _(see above)_
            - `0x0041A212` **_CIsqrt** _(see above)_
          - `0x00404880` **CUnit::BuildOrSerializeCommandBatch** - Gathers selected entities or unit references from an entity's internal lists, builds a local command/target array, and serializes them using a helper routine.
            - `0x00404760` **SerializeCommandBatch** - Iterates over a collection of items, populates a local command packet structure with pointers and metadata, and serializes it using the packet serialization...
              - `0x004087D0` **SerializeCommandPacket** _(see above)_
          - `0x00408610` **CEntity::FindBestTargetOrNode** - Iterates through an entity's internal list of potential pathfinding/target nodes or entries, validates spatial/state constraints, computes distances, and ret...
            - `0x0041A212` **_CIsqrt** _(see above)_
          - `0x0041A212` **_CIsqrt** _(see above)_
          - `0x00406000` **CalculateEntityProximityValue** - Queries nearby entities using a spatial grid and a criteria filter, then loops over the results to accumulate a proximity or distance-based value returned in...
            - `0x0040C950` **ValidateEntityCriteriaFilter** _(address taken, see above)_
            - `0x004177F0` **SpatialGridQueryEntities** _(see above)_
          - `0x00417F80` **CalculateDistance** _(see above)_
        - `0x00406C90` **ProcessEntityCommandBatchOrSpatialQuery** - Inspects an entity's sub-collection of pointers, serializes command batches via SerializeCommandBatch based on internal item flags and counts, and performs a...
          - `0x00404760` **SerializeCommandBatch** _(see above)_
          - `0x0040CAA0` **ValidateAdvancedEntityMatch** _(address taken)_ - Evaluates whether an entity meets criteria and specific internal state/compatibility conditions to qualify for an interaction or query result.
            - `0x0040C950` **ValidateEntityCriteriaFilter** _(see above)_
            - `0x0040CAF0` **ValidateEntityPairCompatibility** _(see above)_
          - `0x004062E0` **CalculateEntityCentroidOrAverage** _(see above)_
          - `0x004177F0` **SpatialGridQueryEntities** _(see above)_
      - `0x004034E0` **UpdateResourceCollection** - Iterates through a list or array of sub-objects stored within a resource structure and invokes a virtual update function on each one.
      - `0x00403520` **ResizeOrCompactContainer** - Manages a dynamic container or vector structure located in the edi register, updating element states, compacting valid items, and growing or reallocating the...
        - `0x00410D00` **UpdateElementState** - Evaluates, validates, and updates a state object or entity (passed via `esi`) against geometry/spatial constraints, invoking virtual callback methods and com...
          - `0x0041A212` **_CIsqrt** _(see above)_
        - `0x004185D0` **AllocateCustomBlock** _(see above)_
        - `0x0041A22A` **memcpy** _(see above)_
      - `0x00408410` **UpdateEntityReferencesAndProximity** - Evaluates global conditions against threshold values to determine an activation state, updates an internal flag at offset 0x1B4, and loops through a collecti...
        - `0x00407FE0` **CheckOrActivateReferences** - Iterates through an array of up to 4 resource handles/references stored in the object, validates their global state and flags, and conditionally either regis...
          - `0x0040BD60` **ResourceObject::ActivateOrRegisterReference** - Searches an internal linked list for an existing sub-object matching the given type identifier; if found, updates its state and returns it, otherwise allocat...
            - `0x004145A0` **CreateSubtypeObject** - Factory function that allocates, initializes, and returns a specific 32-byte subtype object instance based on a given type identifier.
              - `0x004185D0` **AllocateCustomBlock** _(see above)_
              - `0x004144A0` **InitializeSubtypeObject** - Initializes a newly allocated 32-byte object instance by setting its vtable, storing type/parameter identifiers, and initializing member fields based on inpu...
        - `0x004082D0` **UpdateEntitySlotProximityOrReferences** - Validates entity status flags and iterates through an array of up to 4 indexed entity references to check distance thresholds, conditionally clearing or acti...
          - `0x0041A212` **_CIsqrt** _(see above)_
          - `0x004081B0` **ResetOrClearEntityReferences** - Clears base entity state flags and iterates through up to 4 stored reference slots, validating each target entity and conditionally invoking interaction or c...
            - `0x0040DAC0` **ProcessEntityInteractionOrCommand** _(see above)_
          - `0x00407FE0` **CheckOrActivateReferences** _(see above)_
      - `0x004036B0` **UpdateSimulationGridRegions** _(see above)_
      - `0x00403850` **CleanupAndExpireSimulationObjects** - Iterates through two tracked simulation arrays/lists in a state structure, checking a flag (`0x80`) on each object, removing and freeing items matching the c...
        - `0x00403780` **RemoveAndFreeResourceObject** - Unlinks an item from a primary simulation collection (at offset +43D7Ch), handles internal resource pool/handle state updates, removes it from a secondary tr...
          - `0x00417620` **UnlinkListItem** - Unlinks a node from a doubly-linked list structure or index array by updating adjacent node pointers and clearing its fields.
      - `0x00408B60` **ResourceBuffer_FlushOrWritePacket** - Serializes a 1-bit header, copies data into a buffered region with alignment calculation, conditionally flushes via a packet-writing helper if a flag is acti...
        - `0x00418AA0` **BitStream_WriteBits** _(see above)_
        - `0x0041A22A` **memcpy** _(see above)_
        - `0x00408FE0` **WriteBufferPacket** - Writes a data packet with a 5-byte header into a streaming buffer context, flushing and compressing when capacity is exceeded and appending copies to a dynam...
          - `0x00409170` **FlushAndCompressBuffer** - Flushes, compresses, and packetizes the current staging buffer in the context structure, appending a 10-byte header, the payload data (either compressed or u...
            - `0x00418FA0` **CompressLZ77** - Performs LZ77 compression on an input buffer, writing compressed bytes to a global output buffer and returning the total compressed size.
              - `0x00418EC0` **FindMatchLZ77** - Searches a sliding window via hash lookup tables (`0x422C60` and `0x426C60`) to find the longest matching substring for the input stream at the current posit...
            - `0x00418A60` **CalculateCrc32** - Computes the standard CRC32 checksum over a given data buffer, initializing the lookup table on first use.
              - `0x004189C0` **InitCrc32Table** _(see above)_
            - `0x00408F70` **AppendToDynamicBuffer** - Appends data of a given size to a dynamic growable buffer structure, automatically resizing and reallocating the underlying storage if capacity is exceeded.
              - `0x00418670` **ReallocateCustomBlock** _(see above)_
              - `0x0041A22A` **memcpy** _(see above)_
            - `0x004198F2` **__security_check_cookie** _(see above)_
          - `0x00408F70` **AppendToDynamicBuffer** _(see above)_
          - `0x0041A22A` **memcpy** _(see above)_
          - `0x004198F2` **__security_check_cookie** _(see above)_
        - `0x0041A150` **memset** _(see above)_
      - `0x00403940` **ComputeResourceChecksum** - Computes a rolling hash or checksum of a large resource structure, incorporating various integer and floating-point fields across multiple sub-blocks and a d...
      - `0x004090C0` **WriteBufferHeaderOrPacket** - Constructs and writes a 7-byte packet header (comprising values from registers and stack) into a staging buffer context, flushing the buffer first if capacit...
        - `0x00409170` **FlushAndCompressBuffer** _(see above)_
        - `0x00408F70` **AppendToDynamicBuffer** _(see above)_
        - `0x004198F2` **__security_check_cookie** _(see above)_
    - `0x00403C60` **FinalizeResourceBlock** - Iterates through sub-components of a resource block, performing an arithmetic calculation involving table values to compute a validation checksum stored back...
    - `0x00409170` **FlushAndCompressBuffer** _(see above)_
    - `0x00418A60` **CalculateCrc32** _(see above)_
    - `0x00409280` **VerifyResourceBlocks** - Iterates through packed records in a resource block, decompressing or copying sub-components, validating CRC32 checksums, and verifying their expected conten...
      - `0x00419180` **DecompressLZSS** - Decompresses an LZSS-encoded data stream into a fixed 8KB global history buffer (`byte_42ACC8`), returning the decompressed size on success or 0 on error/ove...
      - `0x0041A22A` **memcpy** _(see above)_
      - `0x00418A60` **CalculateCrc32** _(see above)_
    - `0x00403940` **ComputeResourceChecksum** _(see above)_
  - `0x00403F50` **CleanupResourceLists** - Cleans up and releases four distinct arrays of resource/object pointers via virtual destructor calls, updates resource state tables, and frees or recycles a...
  - `0x00401490` **ResourceObject_Cleanup** - Releases and deallocates a large resource block and its associated internal components, managing custom memory-tracked RM header blocks, calling sub-block cl...
    - `0x004013F0` **FreeResourceSubBlock** - Cleans up and deallocates or recycles two custom 'RM' header-tracked memory blocks referenced from offsets within a sub-component structure.
    - `0x00401690` **ResourceObject_Destructor** _(address taken, see above)_
    - `0x0041995F` **vector_destructor** - Destroys an array of objects by invoking a provided element destructor function on each element in forward order, with SEH protection that delegates to __Arr...
      - `0x00419DE8` **__SEH_prolog4** _(see above)_
      - `0x00419E2D` **__SEH_epilog4** _(see above)_
      - `0x00419901` **__ArrayUnwind** _(see above)_
    - `0x004198F2` **__security_check_cookie** _(see above)_
    - `0x0041A224` **__CxxFrameHandler3** _(tail jump, see above)_
  - `0x00418550` **ShutdownResourceBlock** - Cleans up and frees the global resource block along with its linked list of allocations, then resets all related global state variables to zero.
  - `0x00401000` **FreeResourceBlock** _(tail jump, see above)_
  - `0x004198F2` **__security_check_cookie** _(see above)_
  - `0x0041A224` **__CxxFrameHandler3** _(tail jump, see above)_

## Functions

### main - `0x00401050`

_original name `_main` | `int __cdecl main(int argc, char **argv)` | confidence: high | standard program entry point pattern with matching startup and shutdown calls | source: llm_

**Serves as the application's entry point (`main`), hashing command-line arguments to build a configuration seed, initializing core runtime subsystems like sine and CRC32 lookup tables, managing the allocation and initialization of a large resource block, running the RTS simulation loop, and performing a clean teardown.**

The function begins by establishing standard SEH frame handling and stack security cookies. It then iterates over the command-line arguments (`argc`/`argv`), computing a custom rolling hash/checksum value derived from the argument strings (using magic constants like 0x3C6EF372 and 0x811C9DC5 combined with multiplication). It initializes global lookup tables by calling `InitSineTable` and `InitCrc32Table`, and ensures a global flag/state struct (`byte_42285C`) is initialized once. Next, it allocates a custom 0x5BBE4-byte memory block with an 'RM' header and 0xFF block type via `malloc`, initializes it using `memset`, and calls `ResourceBlock_Initialize`. It stores this resource block pointer globally at `0x0042AC60`, invokes `InitializeSimulationAndResourceSubsystem` using a parameter derived from the command-line hash value, and if successful, enters the main RTS simulation via `RtsManager::RunSimulationLoopAndFinalize`. Upon completion, it calls cleanup routines including `CleanupResourceLists`, `ResourceObject_Cleanup`, and `ShutdownResourceBlock`, releases the allocated resource memory (either via `free` or returning it to a custom free list depending on header flags), masks the simulation result to 31 bits, and returns it.

**Parameters:**
- `argc` (int, stack [esp+20h+argc]) - Number of command-line arguments passed to the program.
- `argv` (char**, stack [esp+20h+argv]) - Array of command-line argument strings.

**Returns:** Returns the simulation result code from RtsManager::RunSimulationLoopAndFinalize masked with 0x7FFFFFFF in eax.

**Side effects:**
- Allocates and frees a large global resource memory block ('RM' header) via malloc/free.
- Initializes global lookup tables and tracking counters.
- Runs the primary RTS simulation loop and interacts with game state subsystems.

**Data structures:**
- RM block header: +0x0 word signature (0x4D52 'RM'), +0x2 byte flags/index, +0x3 byte type, +0x4 dword size, +0x8 data payload

**Calls:**
- `InitSineTable` (0x00417ED0) - call at 0x0040113F
- `InitCrc32Table` (0x004189C0) - call at 0x00401144
- `memset` (0x0041A150) - call at 0x004011E0
- `ResourceBlock_Initialize` (0x004012B0) - call at 0x004011F5
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401225
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x0040122F
- `CleanupResourceLists` (0x00403F50) - call at 0x00401236
- `ResourceObject_Cleanup` (0x00401490) - call at 0x00401240
- `ShutdownResourceBlock` (0x00418550) - call at 0x00401289
- `FreeResourceBlock` (0x00401000) - tail jump at 0x0041A3F3
- `__security_check_cookie` (0x004198F2) - call at 0x0041A404
- `__CxxFrameHandler3` (0x0041A224) - tail jump at 0x0041A40E

### ShutdownResourceBlock - `0x00418550`

_original name `sub_418550` | `void __cdecl ShutdownResourceBlock(void)` | confidence: high | standard cleanup function pattern with a linked list traversal and sequential global state reset | source: llm_

**Cleans up and frees the global resource block along with its linked list of allocations, then resets all related global state variables to zero.**

The function checks the global pointer 'Block' (located at 0x00422858). If non-zero, it traverses a singly-linked list where each node's first dword (+0x0) points to the next node. In a loop, it passes each node pointer to the C runtime 'free' function. After freeing all nodes in the list, it sets the global 'Block' pointer and a block of fourteen subsequent global dwords (from 0x00422820 to 0x00422850) and a byte at 0x0042285C to zero.

**Returns:** none

**Side effects:**
- frees memory blocks via MSVCR80!free
- zeroes out global pointer 'Block' (0x00422858)
- zeroes out fourteen contiguous global dwords from 0x00422820 to 0x00422850
- zeroes out byte at 0x0042285C

**Data structures:**
- ResourceBlockNode: +0x0 pointer to next ResourceBlockNode (or NULL)

**Called by:**
- `main` (0x00401050) - call at 0x00401289  
  Called near the end of main during the shutdown phase, prior to restoring the stack frame and returning.

### ResourceObject_Cleanup - `0x00401490`

_original name `sub_401490` | `void __fastcall ResourceObject_Cleanup(void *resource_block)` | confidence: high, clear structure mapping directly to RM resource teardown, custom memory pooling, and vector destruction patterns. | source: llm_

**Releases and deallocates a large resource block and its associated internal components, managing custom memory-tracked RM header blocks, calling sub-block cleanup helpers, and destroying array resources.**

This function performs comprehensive cleanup and teardown of a primary RM resource block passed in as an argument. It systematically inspects multiple pointer fields within the resource structure (at offsets 0x573B0, 0x573A4, 0x57398, 0x5738Ch, and 0x51E90). For each valid pointer, it validates a custom 'RM' magic header word (0x4D52) located 8 bytes before the pointer. It updates global memory tracking statistics (at 0x42281C) and either returns the block to a custom free-list pool (tracked via an array at 0x422820 based on a size/bucket index byte at offset +2) or frees it directly using standard `free()` if the bucket index is 0xFF. Additionally, it invokes a sub-block cleanup routine (`FreeResourceSubBlock`) on a sub-component at offset 0x54740, and uses `vector_destructor` to destroy an array of 7 objects (each 0xC bytes in size starting at offset 0x51EA4) using `ResourceObject_Destructor`. Structured Exception Handling (SEH) and stack cookies are utilized to ensure safe unwinding during destruction.

**Parameters:**
- `resource_block` (void *, edi (passed via stack [esp+4])) - Pointer to the main resource block/structure being destroyed.

**Returns:** none

**Side effects:**
- frees or recycles multiple internal RM-header tracked memory allocations
- modifies global memory tracking counters at 0x42281C and free-list pools at 0x422820
- invokes sub-component teardown and vector destructor on contained resource objects

**Data structures:**
- RM Header: -0x8 word magic (0x4D52), -0x6 byte bucket/index, -0x4 dword size
- Resource Structure: fields at +0x51E90, +0x51EA4 (array of 7 x 0xC objects), +0x54740 (sub-block), +0x5738C, +0x57398, +0x573A4, +0x573B0

**Called by:**
- `main` (0x00401050) - call at 0x00401240  
  Called during cleanup if the resource pointer (esi) is non-zero, passing esi to perform finalization or resource teardown operations.

**Calls:**
- `FreeResourceSubBlock` (0x004013F0) - call at 0x004015ED
- `ResourceObject_Destructor` (0x00401690) - address taken at 0x004015F2
- `vector_destructor` (0x0041995F) - call at 0x00401602
- `__security_check_cookie` (0x004198F2) - call at 0x0041A339
- `__CxxFrameHandler3` (0x0041A224) - tail jump at 0x0041A343

### vector_destructor - `0x0041995F`

_original name `??_M@YGXPAXIHP6EX0@Z@Z` | `void __stdcall vector_destructor(void *ptr, int element_size, int count, void (__cdecl *dtor)(void *))` | confidence: high | standard Microsoft compiler helper function signature and implementation match completely | source: llm_

**Destroys an array of objects by invoking a provided element destructor function on each element in forward order, with SEH protection that delegates to __ArrayUnwind if an exception occurs mid-destruction.**

This function implements the standard MSVC C++ vector destructor helper (mangled as `??_M@YGXPAXIHP6EX0@Z@Z`). It sets up a Structured Exception Handling (SEH) frame and computes the pointer to the end (or rather, shifts the base pointer to point just past the last element by adding element_size * count). It then enters a loop where it decrements the count, moves backward/forward across the array elements, and calls the element destructor `dtor` on each. If an exception interrupts this forward destruction loop before completion (indicated by a completion flag `var_1C` remaining zero), the SEH cleanup filter invokes `__ArrayUnwind` to safely unwind and destroy the successfully constructed preceding elements.

**Parameters:**
- `ptr` (void *, [ebp+arg_0]) - Pointer to the beginning of the array to be destroyed.
- `element_size` (int, [ebp+arg_4]) - Size of a single array element in bytes.
- `count` (int, [ebp+arg_8]) - Number of elements in the array.
- `dtor` (void (__cdecl *)(void *), [ebp+arg_C]) - Pointer to the element destructor function.

**Returns:** none

**Side effects:**
- invokes the provided destructor function on elements of the array
- unwinds partially-destroyed arrays via __ArrayUnwind if an exception occurs

**Called by:**
- `ResourceObject_Cleanup` (0x00401490) - call at 0x00401602  
  Invokes the C++ vector deleting destructor helper (`??_M@YGXPAXIHP6EX0@Z@Z`) to destroy an array of 7 items (each 12 bytes in size) starting at `edi + 0x51EA4`, passing the function pointer `sub_401690` as the element destructor callback.

**Calls:**
- `__SEH_prolog4` (0x00419DE8) - call at 0x00419966
- `__SEH_epilog4` (0x00419E2D) - call at 0x004199A2
- `__ArrayUnwind` (0x00419901) - call at 0x004199BC

### FreeResourceSubBlock - `0x004013F0`

_original name `sub_4013F0` | `void __fastcall FreeResourceSubBlock(void *self)` | confidence: high | disassembly is straightforward and cleanly handles two identical block-release sequences with standard MSVC memory pooling patterns. | source: llm_

**Cleans up and deallocates or recycles two custom 'RM' header-tracked memory blocks referenced from offsets within a sub-component structure.**

The function inspects two distinct pointers located at `[esi + 0x2010]` and `[esi + 0x2004]` within the structure passed via `esi`. For each pointer, if non-null, it checks 8 bytes prior for a custom 'RM' magic header (`0x4D52`). If valid, it decrements a global tracking counter (`dword_42281C`) by the size stored at `header + 4`. It then reads a sub-pool index byte at `header + 2`. If this index is `0xFF`, the memory block is completely freed via `free()`. Otherwise, the block is pushed onto a custom free-list array stored at `dword_422820[index * 4]` for memory pooling/recycling.

**Parameters:**
- `self` (void*, esi) - Pointer to the sub-component structure containing resource pointers at offsets 0x2004 and 0x2010.

**Returns:** none

**Side effects:**
- frees memory or pushes blocks onto a custom freelist pool array at dword_422820
- modifies global tracking counter dword_42281C
- zeroes out the 'RM' header magic word to invalidate freed/recycled blocks

**Data structures:**
- RM Header: +0x0 word magic (0x4D52), +0x2 byte sub-pool index, +0x4 dword size/length
- Sub-component struct: +0x2004 void* resourcePtr1, +0x2010 void* resourcePtr2

**Called by:**
- `ResourceObject_Cleanup` (0x00401490) - call at 0x004015ED  
  Called with the `esi` register initialized to `edi + 0x54740` to perform teardown or cleanup operations on a sub-component within the resource structure.

### CleanupResourceLists - `0x00403F50`

_original name `sub_403F50` | `void __cdecl CleanupResourceLists(void *esi)` | confidence: high | complete control flow and data structure access patterns mapped clearly to standard C++ container cleanup and custom memory tracking. | source: llm_

**Cleans up and releases four distinct arrays of resource/object pointers via virtual destructor calls, updates resource state tables, and frees or recycles a tracked resource memory block marked with the 'RM' signature.**

The function executes several sequential cleanup phases using the state structure passed in `esi`. First, it iterates through an array of objects at offset `+0x57398` (with count at `+0x5739Ch`), invoking a virtual destructor (via function pointer table lookup at `[ecx]`, passing argument `1`) on each non-null entry, then zeroes the count. Second, it processes another list at `+0x5738Ch` (count at `+0x57390h`), updating tracking tables (matching against handle slots at `+0x51E80h`) before similarly calling the virtual destructors and clearing the count. Third and fourth, it iterates through two additional arrays at `+0x573A4h`/`+0x573A8h` and `+0x573B0h`/`+0x573B4h`, destroying any valid objects and resetting their counts to zero. Finally, it inspects a resource allocation located just before the pointer at `+0x51E80h`, validating a 'RM' (0x4D52) magic header and subtracting its size from a global tracking variable (`dword_42281C`). Depending on a control byte at offset `+2`, it either frees the block entirely using `free` or inserts it into a free-list cache (`dword_422820`), resetting the resource pointers in `esi` to null.

**Parameters:**
- `resourceManager` (void*, esi) - Pointer to the main resource/context structure being cleaned up.

**Returns:** none

**Side effects:**
- Invokes virtual destructors on multiple object arrays stored within the resource manager structure.
- Frees or recycles a memory block marked with 'RM' signature via MSVCR80!free or a custom free-list.
- Modifies global variables dword_42281C and dword_422820.
- Zeros out counts and pointers within the resource manager structure.

**Data structures:**
- {"struct_name": "ResourceManager", "fields": [{"offset": "+0x51E80", "type": "void*", "description": "Pointer to resource/slot allocation block"}, {"offset": "+0x51E84", "type": "int", "description": "Resource slot count limit"}, {"offset": "+0x51E88", "type": "short", "description": "Resource index head/free-head"}, {"offset": "+0x51E8C", "type": "int", "description": "Active resource count"}, {"offset": "+0x5738C", "type": "void**", "description": "Array of object pointers (Phase 2)"}, {"offset": "+0x57390", "type": "int", "description": "Count for array at +0x5738C"}, {"offset": "+0x57398", "type": "void**", "description": "Array of object pointers (Phase 1)"}, {"offset": "+0x5739C", "type": "int", "description": "Count for array at +0x57398"}, {"offset": "+0x573A4", "type": "void**", "description": "Array of object pointers (Phase 3)"}, {"offset": "+0x573A8", "type": "int", "description": "Count for array at +0x573A4"}, {"offset": "+0x573B0", "type": "void**", "description": "Array of object pointers (Phase 4)"}, {"offset": "+0x573B4", "type": "int", "description": "Count for array at +0x573B0"}]}

**Called by:**
- `main` (0x00401050) - call at 0x00401236  
  Called unconditionally during the execution phase, following the conditional execution of sub_403E50.

### RtsManager::RunSimulationLoopAndFinalize - `0x00403E50`

_original name `sub_403E50` | `unsigned int __stdcall RtsManager::RunSimulationLoopAndFinalize(void *resourceBlock)` | confidence: high | well-supported by all callee summaries and disassembly sequence | source: llm_

**Runs the RTS simulation ticks until a completion condition is met, flushes and compresses the network/resource buffer, performs checksum and verification passes, and combines the results into a final calculated return value.**

The function checks a flag at resourceBlock + 0x5BBD0. If clear, it enters a loop that repeatedly calls RtsManager::UpdateSimulationTick (sub_403D30) until the tick counter at resourceBlock + 0x5BBC0 reaches or exceeds 0x8CA0, or until the flag at 0x5BBD0 becomes set. Once the loop exits, if the flag is still clear, it invokes FinalizeResourceBlock (sub_403C60). Next, it shifts the pointer forward by 0x54740 bytes and calls FlushAndCompressBuffer (sub_409170). It then computes a CRC32 checksum via CalculateCrc32 (sub_418A60), verifies the resource blocks using VerifyResourceBlocks (sub_409280), and computes a resource checksum using ComputeResourceChecksum (sub_403940). Finally, it performs a series of arithmetic operations, multiplications (imul), rotations (rol), and bitwise manipulations combining the checksums, counters, and internal state fields from the resource block to produce a final combined integer value returned in eax.

**Parameters:**
- `resourceBlock` (void *, [ebp+8]) - Pointer to the main RTS resource and simulation block (approx 0x5BBE4 bytes)

**Returns:** Calculated final integer validation/status value returned in eax

**Side effects:**
- Advances simulation ticks for the RTS manager
- Flushes, compresses, and packetizes staging buffers
- Modifies resource blocks and updates internal state and checksum fields

**Data structures:**
- ResourceBlock: +0x5BBC0 int tick_counter, +0x5BBD0 byte status_flag, +0x5BBD4 int counter_or_state, +0x571CC int field_571CC

**Called by:**
- `main` (0x00401050) - call at 0x0040122F  
  Called conditionally based on the boolean result of sub_401750, passing the resource pointer in esi; its return value is stored in ebx and later used to determine the exit code of main.

**Calls:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403E79
- `FinalizeResourceBlock` (0x00403C60) - call at 0x00403E91
- `FlushAndCompressBuffer` (0x00409170) - call at 0x00403E9E
- `CalculateCrc32` (0x00418A60) - call at 0x00403EBB
- `VerifyResourceBlocks` (0x00409280) - call at 0x00403EC3
- `ComputeResourceChecksum` (0x00403940) - call at 0x00403ECF

### VerifyResourceBlocks - `0x00409280`

_original name `sub_409280` | `bool __cdecl VerifyResourceBlocks(const void *resourceBlock)` | confidence: high | clean parsing loop over structured resource records with well-understood crypto/compression helpers | source: llm_

**Iterates through packed records in a resource block, decompressing or copying sub-components, validating CRC32 checksums, and verifying their expected contents against a destination buffer.**

The function processes a serialized list of resource records from a resource block structure. It loops while parsing 10-byte header entries that specify flags, compressed/uncompressed sizes, and checksums. For each entry, it either decompresses data using LZSS (`DecompressLZSS`) or copies raw data using `memcpy` into a global 8KB buffer (`byte_42ACC8`). It validates a CRC32 checksum via `CalculateCrc32`, checks boundaries, and performs a byte-by-byte or dword-by-dword memory comparison (`memcmp`-like inline unrolled loop) against a target buffer. If all records match successfully and bounds check out, it returns true (1); otherwise, it returns false or 0 upon error.

**Parameters:**
- `resourceBlock` (const void *, [esp+arg_0]) - Pointer to the resource block structure containing metadata, data pointers, and buffer size limits.

**Returns:** bool (1 on success, 0 on failure) in eax

**Side effects:**
- writes decompressed or copied data into global buffer byte_42ACC8

**Data structures:**
- ResourceBlock: +0x2004 src buffer pointer, +0x2008 total size, +0x2010 offset/size field, +0x2014 destination buffer pointer/limit

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403EC3  
  Called with the resource block pointer in ebx and the return value of sub_418A60 passed in esi, performing a subsequent operation on the resource block and returning a byte status in al.

**Calls:**
- `DecompressLZSS` (0x00419180) - call at 0x00409322
- `memcpy` (0x0041A22A) - call at 0x00409334
- `CalculateCrc32` (0x00418A60) - call at 0x0040934B

### DecompressLZSS - `0x00419180`

_original name `sub_419180` | `int __cdecl DecompressLZSS(const void *src, int srcSize)` | confidence: high, matched standard LZSS sliding window decompression algorithm and signature structure. | source: llm_

**Decompresses an LZSS-encoded data stream into a fixed 8KB global history buffer (`byte_42ACC8`), returning the decompressed size on success or 0 on error/overflow.**

The function implements standard LZSS (Lempel-Ziv-Storer-Sachs) decompression, commonly used in games and file archiving (often related to LZRW or similar variants). It takes an input buffer pointer in `eax` and its size in `arg_0`. It maintains a sliding window / history buffer of 0x2000 (8192) bytes located at `byte_42ACC8`. The outer loop processes chunks of compressed data governed by an 8-bit flag byte read from the input stream. For each bit in the flag byte (from bit 0 to 7): if the bit is set, a literal byte is read directly from the input and copied into the history buffer. If the bit is cleared, a 16-bit offset/length match pair is read from the input; the offset and length define a back-reference into the already decoded history buffer, from which bytes are copied sequentially. Rigorous bounds checks are performed throughout to prevent buffer overruns on both the source input and the 8KB destination history buffer. If any bounds check fails, the function immediately halts and returns 0; otherwise, upon successful completion, it returns the total number of bytes written to the history buffer (`ebp`).

**Parameters:**
- `src` (const void *, eax) - Pointer to the compressed source data buffer.
- `srcSize` (int, [esp+18h+arg_0]) - Size of the compressed source data in bytes.

**Returns:** Returns the total number of decompressed bytes stored in the history buffer in eax on success, or 0 on failure/buffer overflow.

**Side effects:**
- Writes decompressed data into the global history buffer byte_42ACC8 (0x0042ACC8).

**Called by:**
- `VerifyResourceBlocks` (0x00409280) - call at 0x00409322  
  Called inside a loop to allocate or retrieve a buffer/pointer (passing an offset/size in eax via stack) when a flag/byte condition (cl == 1) is met; the returned pointer in eax is moved into esi and validated against an expected size.

### FinalizeResourceBlock - `0x00403C60`

_original name `sub_403C60` | `void __stdcall FinalizeResourceBlock(void *resourceBlock)` | confidence: high | clear loop structure, field offsets, and status flag updates matching caller expectations | source: llm_

**Iterates through sub-components of a resource block, performing an arithmetic calculation involving table values to compute a validation checksum stored back into the block.**

The function checks a count at offset +0x5BBBC of the resource block. If the count is greater than zero, it enters a loop running that many times, advancing through a strided array of substructures (spaced by 0x108 bytes). Inside the loop, it checks flags and iterates through a secondary internal table (bounded by addresses 0x41B178 and 0x41BCF4) accumulating values from word fields into an index variable `esi`. It then combines `esi` with values from the substructure at offsets +0xEC, +0xE8, and +0xE4, tracking the maximum calculated value across all iterations. Finally, it stores the resulting best value or -1 into offset +0x5BBD4 and sets a completion flag at offset +0x5BBD0.

**Parameters:**
- `resourceBlock` (void *, [esp+0x10] (arg_0)) - Pointer to the main resource block being finalized.

**Returns:** none

**Side effects:**
- Modifies fields at offsets +0x5BBD0 and +0x5BBD4 within the resource block pointer.

**Data structures:**
- ResourceBlock: +0x5BBBC int count, +0x5BBD0 byte statusFlag, +0x5BBD4 int checksumResult

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403E91  
  Called conditionally if the status flag ([ebx+5BBD0h]) remains zero after the primary processing loop, passing the resource block pointer in ebx for finalization or cleanup.

### RtsManager::UpdateSimulationTick - `0x00403D30`

_original name `sub_403D30` | `void __stdcall RtsManager::UpdateSimulationTick(void *resourceBlock)` | confidence: high | complete integration with caller context, clear callee contracts, and straightforward block layout. | source: llm_

**Executes a single simulation tick for the RTS manager, incrementing counters, processing network stream batches, updating entities, grids, and priority queues, and conditionally flushing network buffers or computing checksums.**

The function begins by reading a frame or tick counter from [ebp+0x5BBC0] and performing modulo arithmetic. If a certain interval is met (e.g., modulo 5 is zero), it processes network stream batches via RtsManager::ProcessNetworkStreamBatches. It then advances a priority queue tick via ProcessPriorityQueueTick, checks simulation bounds/state, updates entities/units via CManager::UpdateEntitiesOrUnits, updates resource collections, and compacts/resizes containers. It updates entity references and proximity based on a floating-point distance threshold. At other intervals (e.g., every 10 ticks), it updates simulation grid regions via UpdateSimulationGridRegions and always cleans up expired simulation objects. Finally, based on further tick conditions (modulo 5 equals 4), it flushes network stream buffers, increments an internal sequence counter at offset 0x5BBC4, periodically computes a resource checksum via ComputeResourceChecksum, and writes packet headers into staging buffers.

**Parameters:**
- `resourceBlock` (void *, [esp+8]) - Pointer to the main RTS manager or simulation resource structure.

**Returns:** none

**Side effects:**
- Increments simulation tick counters at offsets 0x5BBC0 and 0x5BBC4
- Processes network stream batches and flushes resource buffers
- Updates simulation entities, grid regions, and priority queues
- Cleans up and expires simulation objects
- Conditionally computes and stores resource checksums at offset 0x5BBD8

**Data structures:**
- `RtsManager / SimulationBlock`

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403E79  
  Called inside a loop while a status flag ([ebx+5BBD0h]) is zero and a counter ([ebx+5BBC0h]) is less than 0x8CA0, passing the resource block pointer in ebx to process or update resource units.

**Calls:**
- `RtsManager::ProcessNetworkStreamBatches` (0x00408C10) - call at 0x00403D6B
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x00403D80
- `CheckSimulationBoundsOrState` (0x004033A0) - call at 0x00403D87
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x00403D8E
- `UpdateResourceCollection` (0x004034E0) - call at 0x00403D95
- `ResizeOrCompactContainer` (0x00403520) - call at 0x00403D9A
- `UpdateEntityReferencesAndProximity` (0x00408410) - call at 0x00403DAF
- `UpdateSimulationGridRegions` (0x004036B0) - call at 0x00403DC8
- `CleanupAndExpireSimulationObjects` (0x00403850) - call at 0x00403DCF
- `ResourceBuffer_FlushOrWritePacket` (0x00408B60) - call at 0x00403DF4
- `ComputeResourceChecksum` (0x00403940) - call at 0x00403E1C
- `WriteBufferHeaderOrPacket` (0x004090C0) - call at 0x00403E30

### WriteBufferHeaderOrPacket - `0x004090C0`

_original name `sub_4090C0` | `void __fastcall WriteBufferHeaderOrPacket(void *buffer_ctx, uint32_t arg_0, uint32_t count_val)` | confidence: high | structural pattern matches network packet header formatting and dynamic buffer append operations seen in preceding functions | source: llm_

**Constructs and writes a 7-byte packet header (comprising values from registers and stack) into a staging buffer context, flushing the buffer first if capacity is exceeded and appending the header bytes to a secondary dynamic buffer.**

The function begins by establishing a local stack-based byte array (Src) and packing integer and register values into it. Specifically, it extracts bytes from arg_0 (passed on the stack) and ecx, assembling a 7-byte control or packet header. It then checks the buffer offset at offset +0x2000 in the context structure pointed to by esi; if adding 7 bytes exceeds the capacity limit (implied by 2000h), it calls FlushAndCompressBuffer to flush the current buffer. Afterward, it writes the 7 bytes directly into the staging buffer at the current offset, advances the buffer write index by 7, and calls AppendToDynamicBuffer to also append the 7-byte header payload to a dynamic buffer located at offset +0x2010 of the context structure.

**Parameters:**
- `buffer_ctx` (void*, esi) - Pointer to the buffer and packet context structure.
- `ecx_val` (uint32_t, ecx) - Additional data value incorporated into the 7-byte header.
- `arg_0` (uint32_t, stack [esp+0Ch+arg_0]) - Parameter value whose bytes form the first portion of the 7-byte header.

**Returns:** none

**Side effects:**
- Modifies the buffer context state at offset +0x2000 (buffer cursor/size).
- Flushes and compresses the buffer via FlushAndCompressBuffer if space runs out.
- Appends 7 bytes of data to the dynamic buffer at offset +0x2010 via AppendToDynamicBuffer.

**Data structures:**
- BufferContext: +0x2000 uint32_t current_offset, +0x2010 dynamic_buffer_struct

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403E30  
  Called immediately after sub_403940 when the same sub-counter condition is met, passing the sub-buffer pointer [ebp+54740h] in esi, the return value of sub_403940 in ecx, and the counter value on the stack.

**Calls:**
- `FlushAndCompressBuffer` (0x00409170) - call at 0x00409115
- `AppendToDynamicBuffer` (0x00408F70) - call at 0x00409148
- `__security_check_cookie` (0x004198F2) - call at 0x00409157

### ComputeResourceChecksum - `0x00403940`

_original name `sub_403940` | `unsigned int __cdecl ComputeResourceChecksum(void *resourceBlock)` | confidence: high, clear rolling hash algorithm structure processing specific struct offsets and floating-point conversions. | source: llm_

**Computes a rolling hash or checksum of a large resource structure, incorporating various integer and floating-point fields across multiple sub-blocks and a dynamic entity array.**

The function initializes an accumulator using base fields from the resource structure (offsets 0x5BBC0 and 0x51EF8/0x51EFC) and processes them through a repeating mix of multiplications, bit rotations, and XOR operations combined with a magic constant (0x19AB949C). It then loops twice (esi = 2) over a block of fields starting near offset +8, hashing multiple dword values at specific offsets (such as +0x104, +0x108, +0x10C, +0x20C, +0x210, +0x214) with the same hashing mix. Afterward, if the count at offset +0x57390 is greater than zero, it iterates through an array of pointers located at +0x5738C. For each entry, it loads floating-point values from the pointed-to object, manipulates the FPU control word (setting rounding/precision flags via fnstcw/fldcw), converts the floats to integers using fistp, and incorporates them into the rolling checksum along with byte fields at offsets +0x5C and +0x70. The final mixed hash value is returned in eax.

**Parameters:**
- `resourceBlock` (void *, eax) - Pointer to the main resource block containing internal configuration values and a dynamic entity/sub-object list.

**Returns:** unsigned int in eax representing the computed checksum/hash value.

**Data structures:**
- `ResourceBlock`

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403ECF  
  Called with the base resource block pointer in eax (copied from edi) to perform an auxiliary routine on the resource data.
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403E1C  
  Called conditionally when an incremented sub-counter meets a divisibility condition, passing the resource pointer in eax (ebp) and storing its return value into [ebp+5BBD8h].

### ResourceBuffer_FlushOrWritePacket - `0x00408B60`

_original name `sub_408B60` | `void __fastcall ResourceBuffer_FlushOrWritePacket(void *edi_resource_substruct, int param_unused, int src_value)` | confidence: high based on clear register usage, direct callees, and structural offsets matching caller expectations | source: llm_

**Serializes a 1-bit header, copies data into a buffered region with alignment calculation, conditionally flushes via a packet-writing helper if a flag is active, and resets the underlying buffer state via memset.**

The function begins by calling sub_418AA0 [BitStream_WriteBits] to write a single 1-bit flag (value 0) into a bitstream located at [edi + 2824h]. It then computes a byte-aligned or adjusted destination offset using bit-twiddling and multiplication on the provided source size (`[esp+Src]`, adjusted by +2), copying source data into an internal buffer (`[edi + 2024h]`) via `memcpy`. If a condition check at `[edi + 2834h]` is positive (greater than 0), it calls sub_408FE0 [WriteBufferPacket] to transmit the accumulated packet using a base context pointer from `[edi]`. Finally, it resets the internal buffer via `memset` (clearing a region of size 0x800 starting at `[edi + 2024h]`), updates associated buffer descriptor fields within `[edi + 2824h]`, clears the flag at `[edi + 2834h]`, and returns, cleaning up 4 bytes of stack parameters via `retn 4`.

**Parameters:**
- `edi_resource_substruct` (void *, edi) - Pointer to a resource sub-structure containing buffer descriptors and state flags.
- `src_value` (int, [esp+0Ch+Src] / stack) - A size or value parameter passed from the caller, incremented by 2 internally.

**Returns:** none

**Side effects:**
- Writes bits to the bitstream at [edi + 2824h]
- Copies data into internal buffer at [edi + 2024h] using memcpy
- Conditionally calls WriteBufferPacket based on [edi + 2834h]
- Resets internal buffer memory using memset and clears [edi + 2834h]

**Data structures:**
- ResourceSubStruct: +0x0 ptr packet context, +0x2024h buffer [0x800 bytes], +0x2824h bitstream / buffer descriptor, +0x2834h int send/flush flag

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403DF4  
  Called conditionally when [ebp+5BBC0h] % 5 == 4, passing a sub-buffer at [ebp+51F00h] in edi and a value from [ebp+5BBC4h].

**Calls:**
- `BitStream_WriteBits` (0x00418AA0) - call at 0x00408B6F
- `memcpy` (0x0041A22A) - call at 0x00408BB4
- `WriteBufferPacket` (0x00408FE0) - call at 0x00408BCE
- `memset` (0x0041A150) - call at 0x00408BF6

### WriteBufferPacket - `0x00408FE0`

_original name `sub_408FE0` | `void __stdcall WriteBufferPacket(void *buffer_ctx, const void *src, size_t size)` | confidence: high | Clear function structure, well-defined callee interactions and buffer management logic. | source: llm_

**Writes a data packet with a 5-byte header into a streaming buffer context, flushing and compressing when capacity is exceeded and appending copies to a dynamic backup buffer.**

The function packs data into a buffer structure rooted at `esi`. It constructs a 5-byte header on the local stack (`var_C`), consisting of a 1-byte type/flag identifier (from `arg_0`) and a 4-byte length/size descriptor (from `Size`), formatted using little-endian byte representation. It checks if adding the 5-byte header would exceed the 2000-byte threshold (`[esi+2000h]`), and if so, invokes `FlushAndCompressBuffer` (`sub_409170`). It then copies the 5-byte header into the buffer and appends it via `AppendToDynamicBuffer` (`sub_408F70`), incrementing the buffer offset by 5. Next, it checks if adding the payload data (`Size`) would exceed the 2000-byte threshold, flushing again if necessary. If `Size` is less than or equal to 2000, it copies the source data (`Src`) into the buffer via `memcpy`, advances the buffer offset by `Size`, and also appends a copy of the payload via `AppendToDynamicBuffer`. Finally, it increments a packet or operation counter at `[esi+2024h]` by 1.

**Parameters:**
- `buffer_ctx` (void*, esi) - Buffer context pointer containing staging offsets and dynamic storage.
- `arg_0` (uint32_t, [esp+10h]) - Packet type or header identifier flags.
- `Src` (const void*, [esp+14h]) - Pointer to the payload data to be written.
- `Size` (size_t, [esp+18h]) - Size of the payload data in bytes.

**Returns:** none

**Side effects:**
- Modifies buffer context state at [esi+2000h] and increments counter at [esi+2024h]
- Flushes and compresses staging buffer when capacity limits (2000 bytes) are reached
- Appends header and payload blocks to a dynamic growable buffer via sub_408F70

**Data structures:**
- BufferContext: +0x2000 current staging buffer offset/size, +0x2010 dynamic buffer storage, +0x2024 packet/operation count

**Called by:**
- `ResourceBuffer_FlushOrWritePacket` (0x00408B60) - call at 0x00408BCE  
  Called conditionally when `[edi + 2834h] > 0`, passing three arguments (ebx, the newly populated buffer pointer, and esi) to process or dispatch the newly copied resource/simulation data block.

**Calls:**
- `FlushAndCompressBuffer` (0x00409170) - call at 0x0040902A, 0x00409071
- `AppendToDynamicBuffer` (0x00408F70) - call at 0x00409054, 0x00409096
- `memcpy` (0x0041A22A) - call at 0x00409089
- `__security_check_cookie` (0x004198F2) - call at 0x004090AE

### FlushAndCompressBuffer - `0x00409170`

_original name `sub_409170` | `void __fastcall FlushAndCompressBuffer(void *buffer_ctx)` | confidence: high | complete disassembly with well-understood helpers (CompressLZ77, CalculateCrc32, AppendToDynamicBuffer) | source: llm_

**Flushes, compresses, and packetizes the current staging buffer in the context structure, appending a 10-byte header, the payload data (either compressed or uncompressed), and updating CRC checksums.**

The function checks if the buffer pointer at `[edi+0x2000]` is non-zero. If zero, it immediately returns. Otherwise, it calls `CompressLZ77` on the buffer data. If the compressed size is less than the original size, it selects the compressed data and sets a flag byte indicating compression; otherwise, it falls back to the uncompressed buffer and size. It then calculates a CRC32 checksum over the output payload, constructs a 10-byte packet header (containing encoded size, compression flag, and the CRC32 checksum fields), and appends both the header and the payload to the destination stream or buffer using `AppendToDynamicBuffer`. Finally, it updates an overall running CRC32 checksum, increments a sequence/packet counter at `[edi+0x2020]`, and resets the buffer write pointer `[edi+0x2000]` to zero.

**Parameters:**
- `buffer_ctx` (void *, ecx) - Pointer to the buffer context/structure containing state fields, pointers, and offsets.

**Returns:** none

**Side effects:**
- reads from and resets the buffer pointer at context+0x2000
- compresses data using CompressLZ77
- appends headers and payload to the dynamic buffer via AppendToDynamicBuffer
- updates CRC32 checksum storage at context+0x201C
- increments packet sequence count at context+0x2020

**Data structures:**
- `BufferContext`

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403E9E  
  Called with a pointer to a sub-structure within the resource block (ebx + 0x54740) passed in ecx to initialize or service that sub-component.
- `WriteBufferHeaderOrPacket` (0x004090C0) - call at 0x00409115  
  Called with the structure pointer in `esi` when the current write offset at `[esi+0x2000]` plus 7 exceeds 0x2000, likely to flush, resize, or reset the buffer before writing new data.
- `WriteBufferPacket` (0x00408FE0) - call at 0x0040902A, 0x00409071  
  Called with ecx set to the context/buffer object (esi) when appending a header (5 bytes) or payload (edi bytes) would overflow the current buffer capacity (exceeding 2000h bytes), likely to flush, send, or reset the underlying buffer storage.

**Calls:**
- `CompressLZ77` (0x00418FA0) - call at 0x00409194
- `CalculateCrc32` (0x00418A60) - call at 0x004091CF, 0x00409245
- `AppendToDynamicBuffer` (0x00408F70) - call at 0x00409226, 0x00409231
- `__security_check_cookie` (0x004198F2) - call at 0x0040926B

### AppendToDynamicBuffer - `0x00408F70`

_original name `sub_408F70` | `void __fastcall AppendToDynamicBuffer(void *buffer_ctx, void *src, size_t size)` | confidence: high | complete clarity on buffer growth logic, custom struct field offsets, and memcpy/realloc usage. | source: llm_

**Appends data of a given size to a dynamic growable buffer structure, automatically resizing and reallocating the underlying storage if capacity is exceeded.**

The function manages a dynamic buffer structure passed in `ebx`. It reads the current write offset (at `ebx+4`) and the requested data size from the stack. It calculates the required end offset (`current_offset + size`) and compares it against the current buffer capacity (`ebx+8`). If the required capacity exceeds the allocated size, it calculates a new capacity by doubling the previous capacity (starting at a minimum of 0x10) in a loop until it is large enough to accommodate the requested size. It then calls `ReallocateCustomBlock` (`sub_418670`) to resize the underlying memory buffer (`ebx`) and updates the capacity field (`ebx+8`). Finally, it copies the source data (`Src`) into the buffer at the current write offset using `memcpy`, updates the write offset (`ebx+4`) to the new end position, and returns.

**Parameters:**
- `buffer_ctx` (struct BufferContext *, ebx) - Pointer to the buffer context structure managing the dynamic buffer.
- `Src` (const void *, [esp+0x8]) - Pointer to the source data to be appended.
- `Size` (size_t, [esp+0xC]) - Number of bytes to append.

**Returns:** none

**Side effects:**
- Reallocates the buffer memory block if current capacity is insufficient.
- Copies source bytes into the buffer memory.
- Updates the buffer write offset and capacity fields in the context structure.

**Data structures:**
- BufferContext: +0x0 void* buffer_ptr, +0x4 size_t write_offset, +0x8 size_t capacity

**Called by:**
- `WriteBufferHeaderOrPacket` (0x004090C0) - call at 0x00409148  
  Called after copying the 7-byte constructed packet into the buffer, passing a pointer to the local source bytes, the size 7, and a destination pointer at `esi+0x2010`, likely to copy, queue, or log the data chunk to a secondary location.
- `WriteBufferPacket` (0x00408FE0) - call at 0x00409054, 0x00409096  
  Called twice to log, checksum, or process data blocks being written: first with a 5-byte stack-allocated buffer containing the packed header/control bytes, and second with the actual payload pointer (ebp) and length (edi).
- `FlushAndCompressBuffer` (0x00409170) - call at 0x00409226, 0x00409231  
  Called twice during packet flush: first to write the 10-byte constructed header block (from local stack variables `Src`), and second to write the variable-sized payload block (from `ebp` / `Size`) into the underlying output stream or destination.

**Calls:**
- `ReallocateCustomBlock` (0x00418670) - call at 0x00408FA8
- `memcpy` (0x0041A22A) - call at 0x00408FCA

### CalculateCrc32 - `0x00418A60`

_original name `sub_418A60` | `unsigned int __fastcall CalculateCrc32(const void *data, unsigned int size, unsigned int initial_crc)` | confidence: high | standard CRC32 implementation using a lookup table and polynomial 0xEDB88320 | source: llm_

**Computes the standard CRC32 checksum over a given data buffer, initializing the lookup table on first use.**

The function first checks a boolean flag at `byte_42285D` to see if the CRC32 lookup table has been initialized. If not, it calls `InitCrc32Table` to populate it. It then performs standard byte-by-byte CRC32 computation: it takes the initial CRC value passed via `edx` (inverting it initially with `not`), loops through the `esi` bytes starting at memory address `edi`, indexes into the lookup table `dword_422860` using the XOR combination of the current byte and the low byte of the accumulated CRC value, and finally inverts the resulting bit pattern before returning it in `eax`.

**Parameters:**
- `data` (const void*, edi) - Pointer to the buffer over which the CRC32 is calculated.
- `size` (unsigned int, esi) - Size of the data buffer in bytes.
- `initial_crc` (unsigned int, edx) - Initial CRC value (typically 0 for a fresh checksum, or a continuing value).

**Returns:** The computed 32-bit CRC checksum in eax.

**Side effects:**
- Initializes the global CRC32 lookup table at 0x00422860 on its first invocation by calling sub_4189C0.

**Called by:**
- `RtsManager::RunSimulationLoopAndFinalize` (0x00403E50) - call at 0x00403EBB  
  Called with values extracted from the resource block's sub-structure in esi and edi to perform a core operation or calculation, returning a result in eax.
- `VerifyResourceBlocks` (0x00409280) - call at 0x0040934B  
  Called to compute a checksum or hash value over a data buffer (passing the buffer pointer edi and its size in esi/edx), and the returned hash in eax is compared against an expected value stored in the resource record to ensure data integrity.
- `FlushAndCompressBuffer` (0x00409170) - call at 0x004091CF, 0x00409245  
  Called twice (first with edx=0 and an offset/pointer, later with edx=0 and [edi+201Ch]) to compute or update a cryptographic/checksum value (such as a CRC or hash) over the data being flushed, storing the resulting hash/state back into `[edi+201Ch]`.

**Calls:**
- `InitCrc32Table` (0x004189C0) - call at 0x00418A69

### CompressLZ77 - `0x00418FA0`

_original name `sub_418FA0` | `int __cdecl CompressLZ77(const char *input_data, int input_size)` | confidence: high | matched LZ77 compression structure and hashing logic interacting with FindMatchLZ77 | source: llm_

**Performs LZ77 compression on an input buffer, writing compressed bytes to a global output buffer and returning the total compressed size.**

The function initializes two hash tables (at `0x422C60` and `0x426C60`) with `-1` and processes the input data stream via LZ77 compression. It iterates through the input buffer, calling `FindMatchLZ77` (sub_418EC0) to find repeating substrings. If a match is found, it encodes the match distance and length into the output stream (`byte_41F3D8`) and updates the hash tables for the skipped bytes. If no match is found, it processes literals grouped in chunks of up to 8, utilizing a bitmask flag byte to indicate which bytes are literals versus matches. It includes safety checks ensuring output limits (`0x2440` bytes max) are not exceeded.

**Parameters:**
- `input_data` (const char *, [esp+28h+arg_0]) - Pointer to the raw input data buffer to be compressed.
- `input_size` (int, [esp+30h+arg_4]) - Size of the input data buffer in bytes.

**Returns:** Returns the total size of the compressed output data in bytes via eax, or 0 if output buffer bounds are exceeded.

**Side effects:**
- Modifies global LZ77 hash tables at 0x00422C60 and 0x00426C60
- Writes compressed output bytes to global buffer byte_41F3D8

**Called by:**
- `FlushAndCompressBuffer` (0x00409170) - call at 0x00409194  
  Called with `edi` and the buffer pointer/size from `[edi+2000h]` to query or compute a size, length, or capacity value, whose return value in `eax` is stored in `ebx` and used to bound or format the packet header/payload.

**Calls:**
- `FindMatchLZ77` (0x00418EC0) - call at 0x0041901C

### FindMatchLZ77 - `0x00418EC0`

_original name `sub_418EC0` | `int __cdecl FindMatchLZ77(int current_pos, int *match_distance_out)` | confidence: high | Function implements a standard LZ77 hash-chain dictionary match-finding algorithm. | source: llm_

**Searches a sliding window via hash lookup tables (`0x422C60` and `0x426C60`) to find the longest matching substring for the input stream at the current position.**

The function takes a current input position (argument `arg_0` in ebx) and an upper bound/limit (eax), reading stream bytes from a base pointer in edx. First, it ensures there are at least 3 bytes remaining; otherwise, it returns 0. It hashes the first 3 bytes at the current position using a multiplication and right shift (`shr eax, 14h`), then indexes into a hash table at `0x422C60` to retrieve a chain head index (`esi`). It traverses a linked chain of previous occurrences using a second table at `0x426C60`, limiting search depth to 32 iterations (`var_C`). For each candidate position within a valid window distance (< 4096 bytes), it compares bytes to measure the length of the matching string up to a maximum length of 18 bytes (`0x12`). It tracks the best match found so far (`var_8`), writing the match distance to `arg_4` if a better match is found. Finally, if the best match length is at least 3, it returns the match length; otherwise, it returns 0.

**Parameters:**
- `current_pos` (int, stack arg_0 ([esp+10h])) - Current byte index or offset in the input stream.
- `match_distance_out` (int*, stack arg_4 ([esp+14h])) - Pointer where the distance/offset to the best matching sequence is written.

**Returns:** Returns the length of the longest matching sequence found (in eax), or 0 if no match of length >= 3 is found.

**Side effects:**
- Writes the match distance to the integer pointer passed in arg_4 when a valid match is found.

**Data structures:**
- Hash lookup table at 0x422C60 storing initial chain heads
- Hash chain table at 0x426C60 storing next pointers for collision resolution

**Called by:**
- `CompressLZ77` (0x00418FA0) - call at 0x0041901C  
  Called within a compression loop (iterating while esi < edi) to check for a match or find a sequence length/token for the current input position. Passes the input stream pointer/state in edx and edi, current index esi, and a pointer to a local variable output/length storage on the stack. The return value in eax is tested to determine whether a matched sequence or literal byte path should be taken.

### CleanupAndExpireSimulationObjects - `0x00403850`

_original name `sub_403850` | `void __fastcall CleanupAndExpireSimulationObjects(void *simStruct)` | confidence: high based on clear traversal loops, bitflag checks, and cleanup patterns matching resource/simulation lifecycle management | source: llm_

**Iterates through two tracked simulation arrays/lists in a state structure, checking a flag (`0x80`) on each object, removing and freeing items matching the criteria via helper calls or direct virtual destruction.**

The function performs two separate cleanup/expiration scans over collections inside the simulation structure passed in `esi`. 

1. First Loop: Iterates backwards through an array of objects located at `[esi+5738Ch]` with a count at `[esi+57390h]`. For each item, it checks if `byte ptr [eax+18h] & 0x80` is non-zero. If so, it invokes `RemoveAndFreeResourceObject` (`sub_403780`) with `ecx = esi`.

2. Second Loop: Iterates backwards through a second array located at `[esi+573A4h]` with a count at `[esi+573A8h]`. For each item, if `byte ptr [ecx+18h] & 0x80` is set, it performs additional slot/handle validation: it extracts a 16-bit index and 16-bit sequence/generation counter from `[ecx+4]`, validates it against resource pool bounds at `[esi+51E84h]` and `[esi+51E80h]`, resets the resource slot entry, updates free-list linkage (`[esi+51E88h]`, `[esi+51E8Ch]`), compacts the tracked array by shifting elements down, and finally invokes the object's virtual destructor method (`call dword ptr [eax]` with a flag `1` indicating scalar deleting destructor).

**Parameters:**
- `simStruct` (void *, ecx / esi) - Pointer to the main simulation/resource structure

**Returns:** none

**Side effects:**
- frees and unlinks simulation objects whose high bit at offset +0x18 is set
- modifies resource pool tracking tables and counts at offsets +0x51E80 through +0x573A8

**Data structures:**
- Simulation Structure: +0x51E80 resource table array, +0x51E84 resource count, +0x51E88 resource free list head, +0x51E8C resource free count, +0x5738C primary tracked array, +0x57390 primary count, +0x573A4 secondary tracked array, +0x573A8 secondary count
- Tracked Object: +0x4 handle/ID, +0x18 flags byte (0x80 denotes expired/pending removal)

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403DCF  
  Called unconditionally during the update sequence, receiving the resource pointer in esi (ebp).

**Calls:**
- `RemoveAndFreeResourceObject` (0x00403780) - call at 0x00403871

### RemoveAndFreeResourceObject - `0x00403780`

_original name `sub_403780` | `void __fastcall RemoveAndFreeResourceObject(void *simStruct)` | confidence: high, clear control flow, standard resource tracking pool management, and virtual destructor invocation pattern. | source: llm_

**Unlinks an item from a primary simulation collection (at offset +43D7Ch), handles internal resource pool/handle state updates, removes it from a secondary tracked array (at offset +5738Ch), and invokes its virtual destructor/free method.**

The function receives a simulation structure pointer in ecx and first unlinks an element using sub_417620 [UnlinkListItem] with the list base at `[esi+43D7Ch]`. The returned item pointer (in eax) contains handle or index information in its second dword (+4): the lower 16 bits specify a slot index which, if valid and within bounds of the resource pool table (`[esi+51E84h]` / `[esi+51E80h]`), is validated against a sequence/generation tag in the upper 16 bits. If it matches, the pool entry is reset, its generation counter is incremented, and free-list tracking pointers (`[esi+51E88h]`, `[esi+51E8Ch]`) are updated. Next, the function searches for the item pointer within a secondary array at `[esi+5738Ch]` of size `[esi+57390h]`. If found, it removes the item from this array by shifting subsequent elements down and decrementing the count. Finally, it invokes the virtual cleanup/destructor method via the object's virtual table (`[[eax]](1)`), passing 1 to indicate complete deletion/freeing.

**Parameters:**
- `simStruct` (void*, ecx) - Pointer to the main simulation/state structure

**Returns:** none

**Side effects:**
- Unlinks an item from the primary list at simStruct + 43D7Ch
- Updates resource pool allocation state and free list indices at simStruct + 51E80h
- Removes an item entry from the secondary array at simStruct + 5738Ch
- Invokes the virtual destructor/free method on the target object (eax)

**Data structures:**
- Simulation Structure: +43D7Ch primary list base, +51E80h resource pool table, +51E84h pool size, +51E88h free list head/index, +51E8Ch free count, +5738Ch secondary item array, +57390h secondary array count
- Resource/Object node: +0x0 function/data pointer, +0x4 packed handle/index (lower 16 bits = slot index, upper 16 bits = generation tag)

**Called by:**
- `CleanupAndExpireSimulationObjects` (0x00403850) - call at 0x00403871  
  Called during a reverse-order loop over the first collection (`[esi+5738Ch]`) whenever an element's flag byte at offset +18h has its high bit (0x80) set. Passes the primary resource/object pointer in `esi` (via `mov ecx, esi`), acting on individual items that meet the active flag criteria.

**Calls:**
- `UnlinkListItem` (0x00417620) - call at 0x0040378A

### UnlinkListItem - `0x00417620`

_original name `sub_417620` | `void __fastcall UnlinkListItem(void *item, void *listBase)` | confidence: high | standard doubly-linked list node removal routine with array/head integration | source: llm_

**Unlinks a node from a doubly-linked list structure or index array by updating adjacent node pointers and clearing its fields.**

The function checks if the item's list index/state at offset +248h is non-negative. If it is negative, it immediately returns. Otherwise, it checks the previous pointer at offset +244h. If the previous pointer is non-zero, it updates that previous node's next pointer (+240h) to point to the current item's next pointer; otherwise, it updates an array base pointed to by edi using the index stored at +248h. Next, if the item's next pointer (+240h) is non-zero, it updates that next node's previous pointer (+244h) to point to the current item's previous pointer. Finally, it resets the item's next (+240h), previous (+244h), and index (+248h) fields to zero and negative one respectively.

**Parameters:**
- `item` (void*, eax) - Pointer to the list item/node being unlinked.
- `listBase` (void*, edi) - Base pointer used when unlinking head elements referenced via index.

**Returns:** none

**Side effects:**
- modifies adjacent nodes in a doubly-linked list
- clears internal pointers and index fields within the provided item structure at offsets +240h, +244h, and +248h

**Data structures:**
- ListItem: +0x240h next pointer, +0x244h prev pointer, +0x248h index or list state

**Called by:**
- `RemoveAndFreeResourceObject` (0x00403780) - call at 0x0040378A  
  Called at the very beginning of the function with `esi` passed implicitly or via context to obtain a pointer (returned in `eax`) representing the current object or associated data structure whose handle/identifier and lifecycle are being managed.

### UpdateEntityReferencesAndProximity - `0x00408410`

_original name `sub_408410` | `void __stdcall UpdateEntityReferencesAndProximity(float distanceParam)` | confidence: high, based on clean structure iteration, standard __stdcall parameter cleanup (retn 4), and clear calls to known helpers sub_407FE0 and sub_4082D0. | source: llm_

**Evaluates global conditions against threshold values to determine an activation state, updates an internal flag at offset 0x1B4, and loops through a collection of sub-entities to trigger reference checks and proximity updates.**

The function begins by performing two floating-point comparisons against global constants in .rdata, using the resulting FPU status flags to compute a boolean state (stored in `al`). It then compares this computed state with a byte stored at `[esi+1B4h]`. If they differ, `[esi+1B4h]` is updated, and a loop runs from 0 up to `[esi+1B0h]` (representing an entity count). In each iteration of the first loop, if a flag at `[edi+0Bh]` is zero, it invokes `CheckOrActivateReferences` (`sub_407FE0`) passing the computed boolean flag. Following this, a second loop iterates up to `[esi+1B0h]`, loading a floating-point parameter from the caller (passed on the stack via `retn 4`) and invoking `UpdateEntitySlotProximityOrReferences` (`sub_4082D0`) on each entity slot spaced by 0x24 bytes.

**Parameters:**
- `distanceParam` (float, [esp+4]) - Floating-point distance or threshold parameter forwarded to sub_4082D0.

**Returns:** none

**Side effects:**
- Modifies the state byte at [esi+1B4h]
- Iterates through up to 4 entity slots, calling CheckOrActivateReferences and UpdateEntitySlotProximityOrReferences

**Data structures:**
- EntityContainer: +0x1B0 int count, +0x1B4 byte stateFlag, slots starting at offset 0 with stride 0x24

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403DAF  
  Called unconditionally using a structure or buffer located at [ebp+571D4h] with an associated floating-point value loaded onto the FPU stack.

**Calls:**
- `CheckOrActivateReferences` (0x00407FE0) - call at 0x00408471
- `UpdateEntitySlotProximityOrReferences` (0x004082D0) - call at 0x00408499

### UpdateEntitySlotProximityOrReferences - `0x004082D0`

_original name `sub_4082D0` | `void __stdcall UpdateEntitySlotProximityOrReferences(void *entityStruct, float distanceParam)` | confidence: high, clear control flow matching standard entity reference proximity validation patterns seen in earlier functions like sub_4081B0. | source: llm_

**Validates entity status flags and iterates through an array of up to 4 indexed entity references to check distance thresholds, conditionally clearing or activating references based on proximity and entity state.**

The function first checks two boolean flags at offsets +0x0A and +0x0B in the passed `entityStruct` (passed via `edi`). If either is zero, it exits early. Otherwise, it loops 4 times over an array of 32-bit handles/references starting at offset +0x0C. For each valid handle, it decodes an index and a generation/sequence counter, looks up the corresponding entity pointer from a global entity manager table (`dword_42AC60`), and verifies that the target entity is active and valid (checks internal flags at +0x1A and +0x18). It then computes the Euclidean distance between the entities, takes the square root via `_CIsqrt`, and compares it against a threshold from .rdata. If the distance condition is met and the target entity has a non-zero sub-pointer at +0x1F4, a local flag (`var_11`) is set. After checking all 4 slots, if the accumulated conditions fail or `var_11` is set/unset under specific checks, it either invokes `ResetOrClearEntityReferences` (`sub_4081B0`) or `CheckOrActivateReferences` (`sub_407FE0`) depending on a configuration byte at `[ecx+1B4h]` of the parent structure.

**Parameters:**
- `entityStruct` (void*, edi) - Pointer to the primary entity/slot structure being processed.
- `distanceParam` (float, esp+0x20 / FPU stack) - Floating-point parameter passed from the caller for proximity checks.

**Returns:** none

**Side effects:**
- Modifies reference states or clears references via sub_4081B0 / sub_407FE0 based on proximity checks.

**Data structures:**
- EntityStruct: +0x0A bool flag1, +0x0B bool flag2, +0x0C array of 4 packed 32-bit entity handles/references, +0x1B4 config byte.

**Called by:**
- `UpdateEntityReferencesAndProximity` (0x00408410) - call at 0x00408499  
  Called in a secondary loop iterating from 0 up to `[esi+1B0h]` (stepping by 0x24 bytes via `edi`), passing a floating-point value via FPU stack / stack manipulation and `esi` as an argument. This is used to perform a secondary per-element update or calculation phase across the collection using a float parameter.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x0040838A
- `ResetOrClearEntityReferences` (0x004081B0) - call at 0x004083DF, 0x00408403
- `CheckOrActivateReferences` (0x00407FE0) - call at 0x004083F4

### ResetOrClearEntityReferences - `0x004081B0`

_original name `sub_4081B0` | `void __fastcall ResetOrClearEntityReferences(void *entityStruct)` | confidence: high | complete disassembly available, matches caller expectations and standard handle-table lookup patterns. | source: llm_

**Clears base entity state flags and iterates through up to 4 stored reference slots, validating each target entity and conditionally invoking interaction or command handling and floating-point updates on them.**

The function receives a pointer to an entity/waypoint structure in eax. It initializes fields at offset +0x0B and +0x20 to zero, then loops up to 4 times over an array of packed entity references starting at offset +0x0C. For each non-zero reference, it unpacks a low index and high generation/validation word, checks bounds against a global array at 0x0042AC60, retrieves the target entity pointer, verifies its state flags (offset +0x1A == 1 and bit 0 of offset +0x18 not set), and invokes ProcessEntityInteractionOrCommand (sub_40DAC0) with a local parameter structure. Following the call, it performs several floating-point computations (loading, multiplying, adding, and comparing values from the target entity) before advancing to the next reference slot.

**Parameters:**
- `entityStruct` (void*, eax) - Pointer to the primary entity or waypoint structure being processed.

**Returns:** none

**Side effects:**
- Modifies fields at +0x0B and +0x20 of the passed entity structure
- Calls ProcessEntityInteractionOrCommand on referenced sub-entities
- Performs various floating-point operations and state evaluations on valid referenced entities

**Data structures:**
- EntityStruct: +0x0B byte flag, +0x20 dword status, +0x0C dword references[4]

**Called by:**
- `UpdateEntitySlotProximityOrReferences` (0x004082D0) - call at 0x004083DF, 0x00408403  
  Called with `eax` set to `edi` (the current structure pointer) when a validation check fails or when an entity state condition is met. Specifically, it is invoked either during the loop when a distance threshold comparison triggers an early exit path, or after completing the loop successfully if `var_11` is zero and a zero-comparison check passes. It acts as a primary state-clearing, reset, or deactivation handler for the entity/element.

**Calls:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x00408261

### CheckOrActivateReferences - `0x00407FE0`

_original name `sub_407FE0` | `void __fastcall CheckOrActivateReferences(void* this_ptr, void* edx_unused, int flag)` | confidence: high | Based on clear structure iteration patterns, well-understood global resource lookups, and the call to ResourceObject::ActivateOrRegisterReference. | source: llm_

**Iterates through an array of up to 4 resource handles/references stored in the object, validates their global state and flags, and conditionally either registers/activates them or updates a sub-object's completion state.**

The function operates on a structure passed via `eax` (acting as `this_ptr`), adjusting it by an offset of +0x0C to access an array of 4 reference entries. It loops 4 times, reading each reference handle (`ecx`). It unpacks a global resource lookup table from `dword_42AC60`, validates that the reference index and sequence number match the global table entry, and ensures the referenced object (`ebx`) is valid with its state byte `[ebx+1A]` equal to 1 and bit 0 of `[ebx+18]` cleared (not set). Depending on the `flag` argument passed on the stack: if non-zero, it calls `ResourceObject::ActivateOrRegisterReference` with type ID 0x0C; if zero, it traverses a linked list starting at `[ebx+20Ch]`, looking for a node matching specific criteria (byte `[eax+0Ch] == 0Ch` and byte `[eax+0Eh] == 0`), and sets byte `[eax+0Eh] = 1` once found.

**Parameters:**
- `this_ptr` (void*, eax) - Pointer to the parent structure containing the reference array at offset +0x0C.
- `flag` (int, stack [esp+4]) - Boolean or mode flag determining whether to activate/register the resource or update sub-object states.

**Returns:** none

**Side effects:**
- May call ResourceObject::ActivateOrRegisterReference to register or activate a resource object.
- Modifies sub-object state flags (e.g., setting `[eax+0Eh] = 1`) during the traversal if `flag` is zero.

**Data structures:**
- Parent struct: +0x0C contains an array of 4 reference handles (4 bytes each).
- Reference object (`ebx`): +0x18 flags, +0x1A state byte, +0x20Ch pointer to a linked list/sub-object hierarchy.
- Sub-object node: +0x0C type/ID byte, +0x0E status/flag byte, +0x1C next pointer.

**Called by:**
- `UpdateEntityReferencesAndProximity` (0x00408410) - call at 0x00408471  
  Called within a loop iterating from 0 up to `[esi+1B0h]` (where each element is stepped by 0x24 bytes) when `[edi+0Bh]` is zero, passing `eax` as the structure pointer (`edi`) and a boolean flag `ebp` (derived from global/state checks) on the stack. This is used to propagate a state update or flag to individual sub-elements when the main activation state changes.
- `UpdateEntitySlotProximityOrReferences` (0x004082D0) - call at 0x004083F4  
  Called with `eax` set to `edi` and a constant argument `1` pushed onto the stack. This is invoked conditionally at the very end of the function only if the entity passes all 4 reference checks, distance/threshold evaluations, has a non-zero `var_11` flag, and the caller's parent structure at `[ecx+1B4h]` has a non-zero byte flag. It appears to perform a successful activation, trigger, or completion routine for the element.

**Calls:**
- `ResourceObject::ActivateOrRegisterReference` (0x0040BD60) - call at 0x00408051

### ResourceObject::ActivateOrRegisterReference - `0x0040BD60`

_original name `sub_40BD60` | `void* __fastcall ResourceObject::ActivateOrRegisterReference(void* this, void* edx_unused, int typeId, void* extraData)` | confidence: high, clear traversal pattern, standard factory instantiation, and vtable dispatch logic. | source: llm_

**Searches an internal linked list for an existing sub-object matching the given type identifier; if found, updates its state and returns it, otherwise allocates a new one via factory, links it into the list, and initializes it.**

The function first checks a flag bit at `[this+18h]` (where `this` is passed in `ebx`). If bit 0 is cleared (tested via `not cl; test cl, 1`), the function aborts and returns `NULL`. Otherwise, it iterates through a linked list starting at `[this+201h]` (offset +20Ch), checking if any node has matching type ID (`[esi+0Ch] == edi`) and a zeroed status byte (`[esi+0Eh] == 0`). If an existing matching inactive node is found, it calls a virtual method via the vtable at `[esi]` offset +14h, updates its field at `+18h` with a value derived from `extraData`, sets a flag bit (`0x400`) in `[this+18h]`, and returns the node pointer. If no existing match is found, it calls the factory function `CreateSubtypeObject(typeId)` (`sub_4145A0`). If allocation succeeds, it populates `[esi+18h]`, prepends the new node to the linked list at `[this+20Ch]`, invokes its virtual initialization function at vtable offset +4h passing `this`, updates the state flags in `this`, and returns the new node pointer.

**Parameters:**
- `this` (void*, ebx) - The main object instance containing state flags and the linked list root at +20Ch.
- `typeId` (int, ecx) - The type identifier used to search for or create a subtype object.
- `extraData` (void*, stack (arg_4)) - Optional extra data or configuration pointer stored into the subtype object upon activation/creation.

**Returns:** Returns a pointer to the activated or newly created subtype object in eax, or NULL if disabled or allocation failed.

**Side effects:**
- Allocates memory via CreateSubtypeObject if not found in the list
- Modifies object state flags at [this+18h]
- Prepends newly created subtype objects to the linked list at [this+20Ch]
- Invokes virtual methods on the subtype objects

**Data structures:**
- Main Object: +0x18h flags/state word, +0x20Ch pointer to linked list of sub-objects
- Sub-object Node: +0x0 vtable, +0x0Ch type ID, +0x0Eh status/flag byte, +0x18h data/context pointer, +0x1Ch next node pointer

**Called by:**
- `CheckOrActivateReferences` (0x00407FE0) - call at 0x00408051  
  Called conditionally when `arg_0` is non-zero and the current object reference passes all prior handle, index, and state validations (with `eax` set to `1` and `ecx` set to `0Ch`, along with pushed arguments). It acts as an activation, registration, or creation subroutine triggered by the validated entity reference.

**Calls:**
- `CreateSubtypeObject` (0x004145A0) - call at 0x0040BDA1

### CreateSubtypeObject - `0x004145A0`

_original name `sub_4145A0` | `void* __fastcall CreateSubtypeObject(int typeId)` | confidence: high | standard factory switch-case dispatch allocating and initializing objects with specific vtables | source: llm_

**Factory function that allocates, initializes, and returns a specific 32-byte subtype object instance based on a given type identifier.**

The function takes a type identifier in edi (and an unused or context pointer in eax which is saved to esi). It normalizes or bounds-checks the type ID using a lookup table (byte_41C600) to obtain a case index, then uses a switch/jump table (jpt_4145C3) to dispatch to one of seven handlers (or a default path returning null). Each valid case allocates a 32-byte memory block using AllocateCustomBlock, initializes the object via InitializeSubtypeObject, overwrites the virtual function table pointer at offset 0 with a specific vtable from .rdata, and returns the pointer in eax.

**Parameters:**
- `typeId` (int, edi) - Identifier specifying which subtype object variant to instantiate.

**Returns:** Pointer to the newly allocated and initialized object instance in eax, or NULL on failure.

**Side effects:**
- Allocates memory dynamically via custom block allocation

**Data structures:**
- Subtype object: +0x0 vtable pointer, 32 bytes total size

**Called by:**
- `ResourceObject::ActivateOrRegisterReference` (0x0040BD60) - call at 0x0040BDA1  
  Called when an existing matching node is not found in the linked list at offset +20Ch of the object (ebx), acting as an allocation or creation routine to instantiate a new node or resource. The returned pointer (in eax, later moved to esi) is checked for success and subsequently integrated into the linked substructure if non-zero.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004145CF, 0x004145F8, 0x00414621, 0x0041464A, 0x00414673, 0x00414698, 0x004146BD
- `InitializeSubtypeObject` (0x004144A0) - call at 0x004145E6, 0x0041460F, 0x00414638, 0x00414661, 0x00414686, 0x004146AB, 0x004146D0

### InitializeSubtypeObject - `0x004144A0`

_original name `sub_4144A0` | `void __fastcall InitializeSubtypeObject(void *objectInstance, int typeId)` | confidence: high | clean disassembly with well-understood object initialization patterns | source: llm_

**Initializes a newly allocated 32-byte object instance by setting its vtable, storing type/parameter identifiers, and initializing member fields based on input registers EAX, ECX, and ESI.**

The function takes a pointer to a newly allocated object instance in EAX and an integer identifier/parameter in ECX. It initializes the object header by storing a default vtable pointer (`off_41D7D8`) at offset `+0` and zeroing out offset `+4`. It stores ECX at offset `+0Ch` and computes a lookup/lookup-table offset using a clamped/masked version of ECX combined with `byte_41C600`, storing the result at offset `+8`. Based on the value of ESI (clamped between 1 and 3), it calculates and stores a byte value at offset `+0Dh`. Finally, it clears bytes at offsets `+0Eh`, `+0Fh`, and double-words at `+18h` and `+1Ch` to zero.

**Parameters:**
- `objectInstance` (void *, eax) - Pointer to the newly allocated 32-byte object to initialize
- `typeId` (int, ecx) - Identifier or parameter value used to configure type-specific fields

**Returns:** none

**Side effects:**
- Initializes fields of the object instance passed in EAX

**Data structures:**
- Object instance: +0x0 vtable ptr, +0x4 int/ptr field, +0x8 int/ptr field, +0xC char typeId, +0xD char clampedSsi, +0xE char, +0xF char, +0x18 int, +0x1C int

**Called by:**
- `CreateSubtypeObject` (0x004145A0) - call at 0x004145E6, 0x0041460F, 0x00414638, 0x00414661, 0x00414686, 0x004146AB, 0x004146D0  
  Called immediately after successful memory allocation and temporary storage preparation to initialize the newly allocated object instance (with the raw pointer in EAX and identifier/parameter passed via ECX), prior to writing the type-specific vtable/metadata pointer into the object header.

### ResizeOrCompactContainer - `0x00403520`

_original name `sub_403520` | `void __fastcall ResizeOrCompactContainer(void *pContainer)` | confidence: high | Clear iteration, compaction, and vector resizing/reallocation logic matching standard C++ STL vector-like patterns. | source: llm_

**Manages a dynamic container or vector structure located in the edi register, updating element states, compacting valid items, and growing or reallocating the underlying buffer when capacity is exceeded.**

The function processes a container passed via the edi register. First, it iterates through the active elements (count at edi+0x5739c, array pointer at edi+0x57398) and invokes UpdateElementState (sub_410D00) on each. Next, it performs a compaction pass: it iterates again over the elements, checking a flag byte at offset +0x29 of each element object. If the flag is non-zero, it invokes a virtual destructor/cleanup method at [object_vtable] and drops the item from the sequence; otherwise, it keeps the item and packs it down. After compaction, it checks if the new active element count exceeds the capacity stored at edi+0x573a0. If capacity is insufficient, it computes a new capacity (doubling the current capacity or using a default size of 16), allocates a new memory block via AllocateCustomBlock (sub_4185D0), copies the existing elements using memcpy, and frees or recycles the old memory block through custom allocator tracking or standard free(). Finally, it updates the container's internal pointer, count, and capacity fields.

**Parameters:**
- `pContainer` (void*, edi) - Pointer to the container or resource structure being updated and resized.

**Returns:** none

**Side effects:**
- updates element states via sub_410D00
- frees or reallocates internal storage buffers using AllocateCustomBlock and free

**Data structures:**
- Container: +0x57398 element array pointer, +0x5739C active element count, +0x573A0 capacity
- Element: +0x29 state/flag byte, +0x0 virtual function table pointer

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D9A  
  Called unconditionally as part of the update sequence following sub_4034E0.

**Calls:**
- `UpdateElementState` (0x00410D00) - call at 0x00403543
- `AllocateCustomBlock` (0x004185D0) - call at 0x004035DD, 0x00403614
- `memcpy` (0x0041A22A) - call at 0x0040362D

### UpdateElementState - `0x00410D00`

_original name `sub_410D00` | `bool __stdcall UpdateElementState(void *pElement)` | confidence: medium | The disassembly shows a complex mixture of global table lookups, virtual method calls, and extensive floating-point math typical of 3D game entity updates, ray/spatial intersection tests, or collision handling. | source: llm_

**Evaluates, validates, and updates a state object or entity (passed via `esi`) against geometry/spatial constraints, invoking virtual callback methods and computing vector distance calculations.**

The function begins by checking a boolean flag at `[esi+29h]`. If true, it skips validation and returns true. Otherwise, it resolves an object reference from a global manager table (`0x0042AC60`) using an index derived from `[esi+0Ch]`. It validates the retrieved object using a bitwise check on its byte at offset `+18h`, then invokes a virtual method via the vtable at `[esi]` offset `+8`. If successful, it copies certain fields (`+8` and `+0Ch`) into `[esi+18h]` and `[esi+1Ch]`. It then performs extensive floating-point arithmetic—including computing coordinate differences, dot products, vector magnitudes, square roots via `_CIsqrt`, and comparisons against constants in `.rdata`—to determine whether the element satisfies spatial or geometric criteria. If conditions are met, it updates internal fields, calls another virtual function via the vtable at offset `+4`, and sets the completion flag at `[esi+29h]` to 1.

**Parameters:**
- `pElement` (void *, esi (passed via register from caller, though stack has retn 4 indicating a __stdcall convention or custom wrapper)) - Pointer to the object/structure being updated.

**Returns:** al (boolean indicating success or whether the operation completed)

**Side effects:**
- Modifies fields within the object structure passed in esi (including flags at +29h, cached coordinates, and state data)
- Invokes virtual methods through the object's vtable

**Data structures:**
- Object structure (esi): +0Ch handle/index, +10h-17h spatial coordinates/vectors, +18h cached target values, +29h status/completion flag

**Called by:**
- `ResizeOrCompactContainer` (0x00403520) - call at 0x00403543  
  Called inside a loop that iterates over elements stored in an array at [edi+57398h], performing a per-element cleanup or update operation.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00410DA9

### UpdateResourceCollection - `0x004034E0`

_original name `sub_4034E0` | `void __fastcall UpdateResourceCollection(void *resourceObj)` | confidence: high, clear traversal pattern and virtual method dispatch. | source: llm_

**Iterates through a list or array of sub-objects stored within a resource structure and invokes a virtual update function on each one.**

The function expects a resource structure pointer in the `edi` register. It reads a count located at offset `+0x57390` and a pointer to an array/collection at offset `+0x5738C`. If the count is greater than zero, it enters a loop from `esi = 0` up to `count - 1`. In each iteration, it fetches the object pointer from the array at index `esi`, retrieves its virtual method table (vftable) from the object's offset `+0`, gets the function pointer from the vftable at offset `+4`, and calls it with the object pointer passed as `ecx` (standard __thiscall convention). The loop also performs an extraneous floating-point load and store (`fld` / `fstp`) during each iteration, likely reflecting an inline or optimized constant handling.

**Parameters:**
- `resourceObj` (void*, edi) - Pointer to the main resource structure containing the collection and count.

**Returns:** none

**Side effects:**
- Calls virtual functions on multiple sub-objects contained within the resource structure.

**Data structures:**
- ResourceStructure: +0x5738C pointer to array of object pointers, +0x57390 integer count of elements

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D95  
  Called unconditionally in sequence after sub_403460, receiving the resource pointer in edi (ebp).

### CManager::UpdateEntitiesOrUnits - `0x00403460`

_original name `sub_403460` | `void __fastcall CManager::UpdateEntitiesOrUnits(void *managerStruct, void *edx_unused)` | confidence: high | complete disassembly available with well-understood callee signatures and structural offsets. | source: llm_

**Iterates through an array of entity/unit structures within a manager object, conditionally performing update ticks and invoking a sequence of combat, AI, ability, and state-processing subroutines based on a stride of 4FCh bytes.**

The function loops a number of times specified by a counter at `[esi + 0x5BBBCh]`. In each iteration, it computes a modulated index using `[esi + 0x5BBC0h]` plus an incrementing offset (`ebx`), dividing it by 10 (`0x0A`). If the remainder is zero, it performs a validity check using a global table/pointer (`dword_42AC60`) and bitflags before executing a large suite of per-entity update and processing functions on the array element located at `edi` (`esi + 0x630h + ebp * 0x4FCh`). These invoked subroutines handle entity selection/lists, combat or ability updates, AI or production actions, timed behaviors, unit abilities, behavior states, and command batch/spatial processing.

**Parameters:**
- `managerStruct` (void*, esi) - Pointer to the main manager/resource structure containing counts and an array of entity/unit sub-structures.

**Returns:** none

**Side effects:**
- Iterates over and updates a collection of entities/units within the manager structure.
- Triggers state machine updates, combat evaluation, ability processing, and spatial/command updates for active units.

**Data structures:**
- Manager struct: +0x5BBBCh int entityCount, +0x5BBC0h int tickOffset, +0x630h array of entity/unit sub-structures (stride 0x4FCh).

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D8E  
  Called unconditionally in sequence after sub_4033A0, receiving the resource pointer in esi (ebp).

**Calls:**
- `UpdateEntitySelectionAndLists` (0x00404940) - call at 0x0040349F
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x004034A4
- `Entity::ProcessAIOrProductionAction` (0x00404D20) - call at 0x004034AB
- `Entity::ProcessTimedBehaviorsOrActions` (0x00405040) - call at 0x004034B1
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x004034B6
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x004034BD
- `ProcessEntityCommandBatchOrSpatialQuery` (0x00406C90) - call at 0x004034C3

### ProcessEntityCommandBatchOrSpatialQuery - `0x00406C90`

_original name `sub_406C90` | `void __stdcall ProcessEntityCommandBatchOrSpatialQuery(void *entity)` | confidence: high | clean disassembly with well-understood caller/callee context and standard MSVC calling conventions. | source: llm_

**Inspects an entity's sub-collection of pointers, serializes command batches via SerializeCommandBatch based on internal item flags and counts, and performs a spatial grid query using SpatialGridQueryEntities to process nearby targets.**

The function performs a series of checks and operations on a given entity structure. First, it tests spatial or type conditions using floating-point comparisons against constants in .rdata, verifying that the entity is not in a specific state ([ebp+0Ch] != 3). It then iterates up to [ebp+3C8h] times over a sub-collection starting at offset +0x248, inspecting entity pointers. Depending on status flags (bit 0x10 and bit 0x4000 in offset +0x1C) and internal counters, items are categorized into local arrays (command batches). If enough items are collected, it invokes SerializeCommandBatch. Next, if sufficient secondary command targets exist, it calculates the entity's centroid using CalculateEntityCentroidOrAverage and queries a spatial grid via SpatialGridQueryEntities to find nearby entities. It loops through the resulting query hits, performs conditional validation, and issues a final command batch serialization via SerializeCommandBatch.

**Parameters:**
- `entity` (void *, [esp+0E8h]) - Pointer to the main simulation or entity object being processed.

**Returns:** none

**Side effects:**
- Serializes command packets via SerializeCommandBatch
- Queries the spatial grid via SpatialGridQueryEntities

**Data structures:**
- Entity: +0x0 vtable/ptr, +0x0Ch state/type flag, +0x34 command metadata, +0x248 sub-collection array, +0x3C8 item count

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034C3  
  Called with `edi` pushed as an argument at the end of the update subroutine chain for the current entity.

**Calls:**
- `SerializeCommandBatch` (0x00404760) - call at 0x00406D77, 0x00406EC3
- `ValidateAdvancedEntityMatch` (0x0040CAA0) - address taken at 0x00406DAD
- `CalculateEntityCentroidOrAverage` (0x004062E0) - call at 0x00406DCE
- `SpatialGridQueryEntities` (0x004177F0) - call at 0x00406DE4

### ValidateAdvancedEntityMatch - `0x0040CAA0`

_original name `sub_40CAA0` | `__thiscall bool ValidateAdvancedEntityMatch(const void *entity, const void *queryCriteria)` | confidence: high | standard callback wrapper filtering based on entity criteria, internal state flags, and pair compatibility. | source: llm_

**Evaluates whether an entity meets criteria and specific internal state/compatibility conditions to qualify for an interaction or query result.**

The function first calls ValidateEntityCriteriaFilter to check if the entity matches the general query criteria. If it fails, it returns false. If it passes, it inspects several bitfields and flags on the entity structure: bit 0 of the byte at offset 0x18 must be set (inverted via NOT and tested with bitwise AND 1, implying bit 0 must be 0 originally or after negation), certain bits in the word at offset 0x18 (0x206) must be clear, and the byte at offset 0x60 must be zero. If all internal state checks pass, it extracts a pointer from the criteria structure ([edi]) and calls ValidateEntityPairCompatibility between the entity and the secondary entity/context pointer, returning the resulting boolean compatibility status.

**Parameters:**
- `entity` (const void *, esp+4 (arg_0)) - The primary entity being evaluated.
- `queryCriteria` (const void *, esp+8 (arg_4)) - The query criteria or context structure containing search parameters and associated data pointers.

**Returns:** bool (in al), true if all criteria and compatibility checks pass, false otherwise.

**Data structures:**
- `Entity`
- `QueryCriteria`

**Called by:**
- `ProcessEntityCommandBatchOrSpatialQuery` (0x00406C90) - address taken at 0x00406DAD  
  Address is passed as a callback or comparator function pointer into a configuration or search helper routine (called via sub_4062E0) along with a local buffer (`var_C8`) and data pointers.

**Calls:**
- `ValidateEntityCriteriaFilter` (0x0040C950) - call at 0x0040CAAC
- `ValidateEntityPairCompatibility` (0x0040CAF0) - call at 0x0040CADC

### CUnit::UpdateAIStateOrBehavior - `0x00406760`

_original name `sub_406760` | `void __fastcall CUnit::UpdateAIStateOrBehavior(void *entity)` | confidence: high | based on clear structure layout access patterns, integration with known unit command/targeting functions, and state machine switch-case patterns. | source: llm_

**Evaluates and updates an entity's high-level state machine or behavior mode (stored at offset +0x0C) by checking environmental conditions, proximity values, target validity, and executing corresponding command batches.**

The function begins by calling CalculateEntityStatsOrAttributes and CalculateEntityCentroidOrAverage on the entity (passed in eax). It then calculates directional vectors and normalizes them using Vec3_NormalizeOrZero. It checks various flags and iterates through internal entity lists (starting at offset +0x3D0 up to +0x3DCh elements) to find maximum values and evaluate target conditions. Depending on the current state stored at `entity+0x0C`, it branches via a jump table (jpt_4068CF) to handle distinct behavior states (e.g., idle, approaching, attacking, fleeing). Within these state handlers, it calls helper functions such as FindNearestValidEntityOrTarget, CEntity::FindBestTargetOrNode, CalculateEntityProximityValue, CalculateDistance, and CUnit::BuildOrSerializeCommandBatch to trigger actions, transition states, or issue commands based on spatial and combat criteria.

**Parameters:**
- `entity` (void*, eax) - Pointer to the entity or unit structure being updated.

**Returns:** none

**Side effects:**
- Modifies unit state fields within the entity structure, notably at offset +0x0C, +0x14, +0x18, and +0x1C
- Invokes command serialization and queuing routines via CUnit::BuildOrSerializeCommandBatch

**Data structures:**
- Entity: +0x0C state/mode (int), +0x14 counter/stat, +0x18 target/node index, +0x1C boolean flag, +0x2C target/position data, +0x3C count/limit, +0x3CC threshold, +0x3D0 internal list, +0x3DC list count

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034BD  
  Called with `eax` set to `edi` (the current entity structure) during the periodic update block for the entity.

**Calls:**
- `CalculateEntityStatsOrAttributes` (0x00405AD0) - call at 0x0040677D
- `CalculateEntityCentroidOrAverage` (0x004062E0) - call at 0x0040678C
- `Vec3_NormalizeOrZero` (0x00417FD0) - call at 0x004067CB
- `FindNearestValidEntityOrTarget` (0x00406540) - call at 0x004068F7, 0x00406B21
- `CUnit::BuildOrSerializeCommandBatch` (0x00404880) - call at 0x00406920, 0x00406989, 0x004069F5, 0x00406A6D, 0x00406A87, 0x00406B5D, 0x00406B96, 0x00406C3A (+1 more)
- `CEntity::FindBestTargetOrNode` (0x00408610) - call at 0x00406957
- `_CIsqrt` (0x0041A212) - call at 0x004069D0, 0x00406BD2
- `CalculateEntityProximityValue` (0x00406000) - call at 0x00406A2F, 0x00406AA6
- `CalculateDistance` (0x00417F80) - call at 0x00406B00

### CalculateEntityProximityValue - `0x00406000`

_original name `sub_406000` | `float __stdcall CalculateEntityProximityValue(void *entity, void *grid_data)` | confidence: high based on clear API usage of SpatialGridQueryEntities and ValidateEntityCriteriaFilter | source: llm_

**Queries nearby entities using a spatial grid and a criteria filter, then loops over the results to accumulate a proximity or distance-based value returned in st(0).**

The function allocates local storage for up to 48 (30h) entities and calls SpatialGridQueryEntities to retrieve entities surrounding the input entity (whose pointer is passed as arg_0). It passes the ValidateEntityCriteriaFilter function pointer as the filtering callback. It then iterates through the returned array of entity pointers in chunks of 4 (unrolled loop) and remainder elements, checking entity flags (+0x18, +0x1C) and performing floating-point calculations to accumulate a value based on spatial proximity or relative metrics. Finally, it cleans up the stack and returns the accumulated float value in st(0).

**Parameters:**
- `entity` (void *, [esp+0ECh + arg_0]) - Pointer to the reference entity around which the spatial query and proximity calculations are performed.
- `grid_data` (void *, passed via global/EDX setup) - Spatial grid data structure used for querying nearby entities.

**Returns:** Accumulated floating-point proximity/distance value returned in st(0).

**Data structures:**
- Entity: +0x18 byte flags, +0x1C pointer to sub-struct with byte at +0xC

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x00406A2F, 0x00406AA6  
  Called at 0x00406A2F and 0x00406AA6 with the entity pointer in eax and a pointer to a local stack variable, used to compute distance, range, or angle relative to a target.

**Calls:**
- `ValidateEntityCriteriaFilter` (0x0040C950) - address taken at 0x00406029
- `SpatialGridQueryEntities` (0x004177F0) - call at 0x00406055

### CEntity::FindBestTargetOrNode - `0x00408610`

_original name `sub_408610` | `int __stdcall CEntity::FindBestTargetOrNode(void *entity_ptr, int arg0, int arg1)` | confidence: high | clear loop structure, field offsets, and spatial proximity validation routines | source: llm_

**Iterates through an entity's internal list of potential pathfinding/target nodes or entries, validates spatial/state constraints, computes distances, and returns the index of the best match or -1 if none qualifies.**

The function accesses an array/list of structures stored at offset +0x1B0 of the entity passed in eax. It loops over a count of entries, performing various checks on each element: verifying status flags, index bounds against a global entity/node manager table (`dword_42AC60`), active object states, and distance calculations using square roots (_CIsqrt). If an entry meets all proximity and state criteria, its index is recorded as the best candidate, eventually returning the winning index or -1.

**Parameters:**
- `entity_ptr` (void *, eax) - Pointer to the current simulation entity object
- `arg_0` (int, stack [ebp+0x8]) - An argument used in distance or coordinate threshold checks
- `arg_8` (int, stack [ebp+0xC]) - An argument compared against an entity-internal byte field during iteration

**Returns:** Returns the index of the selected target/node (int in eax), or -1 if no suitable entry is found.

**Data structures:**
- Entity: +0x1B0 int count, +0x1B4 array of node/target structures (each 0x24 bytes wide)

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x00406957  
  Called at 0x00406957, passing an entity array/substructure pointer, a stack variable pointer, and an index argument, to search or query related target/pathfinding nodes and return an index or status code.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00408745

### CUnit::BuildOrSerializeCommandBatch - `0x00404880`

_original name `sub_404880` | `void __stdcall CUnit::BuildOrSerializeCommandBatch(int arg_0, int arg_4)` | confidence: medium | Based on struct member offsets, stack buffer assembly, and a call to the known serialization routine sub_404760. | source: llm_

**Gathers selected entities or unit references from an entity's internal lists, builds a local command/target array, and serializes them using a helper routine.**

The function inspects an entity instance (passed in ebx) to build a batch of unit pointers or command targets. First, if `[ebx+0x3C8]` is greater than 0, it copies a primary pointer/value located at `[ebx+0x248]` into a local stack buffer (`var_194`). Next, it iterates through a list of up to `[ebx+0x3DCh]` elements starting at `[ebx+0x3D0]`, checking a flag (`not cl & 1`) on each element's internal structure; if valid, it appends the pointer to the local stack buffer. If any items were gathered, it calls `SerializeCommandBatch` (`sub_404760`) passing the local buffer, the count, and parameters from the caller (`arg_0`, `arg_4`). Finally, it loads a floating-point value from a global location (`0x0042AC60`), manipulates the FPU stack, and returns.

**Parameters:**
- `arg_0` (int, [esp+0x1A4+arg_0]) - Passed as an argument to the serialization routine sub_404760.
- `arg_4` (int, [esp+0x1A4+arg_4]) - Passed as an argument to the serialization routine sub_404760.

**Returns:** none

**Side effects:**
- Calls sub_404760 to serialize a constructed command batch
- Performs FPU operations using global 0x0042AC60

**Data structures:**
- CUnit entity layout: +0x248 target/pointer field, +0x3C8 count/flag, +0x3D0 array of pointers, +0x3DC array size

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x00406920, 0x00406989, 0x004069F5, 0x00406A6D, 0x00406A87, 0x00406B5D, 0x00406B96, 0x00406C3A (+1 more)  
  Called across multiple branches (at 0x00406920, 0x00406989, 0x004069F5, 0x00406A6D, 0x00406A87, 0x00406B5D, 0x00406B96, 0x00406C3A, and 0x00406C55) with constant integers (like 3 or 5) and pointers to entity substructures or coordinate data, acting as a utility routine to dispatch events, trigger animations/effects, or update entity subsystems.

**Calls:**
- `SerializeCommandBatch` (0x00404760) - call at 0x004048F8

### SerializeCommandBatch - `0x00404760`

_original name `sub_404760` | `void __stdcall SerializeCommandBatch(const void *arg_0, void *arg_4, int count, char arg_C, int arg_10, const void *arg_14)` | confidence: high | standard stack-based arguments and control flow matching caller expectations and packet serialization logic | source: llm_

**Iterates over a collection of items, populates a local command packet structure with pointers and metadata, and serializes it using the packet serialization routine.**

The function initializes a local buffer/structure on the stack representing a command packet. It iterates up to 'count' times, extracting pointers from an input array (arg_4) and copying metadata bytes from arg_0, storing them into the local packet structure. It checks network/buffer state limits using a global tracking structure at dword_42AC60, updating packet counts and error flags if capacity is exceeded. Finally, it invokes sub_4087D0 [SerializeCommandPacket] to serialize the constructed packet structure into a bitstream.

**Parameters:**
- `arg_0` (const void*, [esp+50h+arg_0]) - Source pointer providing header/type bytes for the command packet.
- `arg_4` (void**, [esp+50h+arg_4]) - Array of item pointers to be processed and packed into the command.
- `count` (int, [esp+50h+arg_8]) - Number of items in the collection / iteration limit.
- `arg_C` (char, [esp+50h+arg_C]) - Configuration flag incorporated into the command packet header.
- `arg_10` (int, [esp+50h+arg_10]) - Integer parameter/ID included in the packet structure.
- `arg_14` (const void*, [esp+50h+arg_14]) - Additional pointer source providing secondary payload/address fields for the packet.

**Returns:** none

**Side effects:**
- Serializes command packets into a global bitstream/buffer via sub_4087D0
- Modifies global statistics and state counters within the structure at dword_42AC60

**Called by:**
- `ProcessEntityCommandBatchOrSpatialQuery` (0x00406C90) - call at 0x00406D77, 0x00406EC3  
  Called twice (at 0x00406D77 and 0x00406EC3) to process a filtered batch of entity elements. In the first call, it passes the main entity pointer (`ebp`), a local array of collected element pointers (`ecx`), the count (`esi`), and constant configuration values (3, 0, and `ebp+34h`) when elements are found. In the second call, it passes the main entity pointer (`ebp`), a secondary collection (`var_60`), a count (`var_DC`), and pointer/integer arguments (`4`, `ecx+8`, `ecx+4`) to perform a subsequent batch operation.
- `CUnit::BuildOrSerializeCommandBatch` (0x00404880) - call at 0x004048F8  
  Called if any matching items were found/collected (esi > 0), passing a pointer to the local entity pointer array on the stack, the entity pointer (ebx), the count (esi), and two additional arguments (arg_0 and arg_4) passed into the current function.

**Calls:**
- `SerializeCommandPacket` (0x004087D0) - call at 0x00404852

### FindNearestValidEntityOrTarget - `0x00406540`

_original name `sub_406540` | `bool __stdcall FindNearestValidEntityOrTarget(void *entity, void *outTarget)` | confidence: high | based on clear loop structures, spatial distance calculations, filtering checks, and interaction with global entity management tables. | source: llm_

**Scans entity lists and grid structures to find a valid target or entity meeting specific distance and filtering criteria relative to the given entity.**

The function iterates through two separate potential entity/target collections retrieved from a global management structure (`dword_42AC60`). In the first loop, it iterates up to a count stored at `[edx+57390h]`, retrieving entries via a pointer array at `[edx+5738Ch]`. For each entry, it checks various entity flags and types, applies grid-based spatial filtering via `CheckEntityGridFilter`, and calculates the Euclidean distance to the source entity using `CalculateDistance`. If the distance is within valid bounds and closer than previous candidates, it updates the output structure `outTarget` and sets a found flag. If no valid target is chosen in the first loop, a second loop scans an alternate list up to `[edx+5BBBCh]`, performing similar filtering and manual Euclidean distance calculations using `_CIsqrt`, updating `outTarget` if a closer match is found. Finally, it returns true if a valid target was found and selected, or false otherwise.

**Parameters:**
- `entity` (void *, [ebp+8]) - Pointer to the source entity data being evaluated.
- `outTarget` (void *, [ebp+0xC]) - Pointer to a destination buffer/structure where the nearest valid target reference is written if found.

**Returns:** bool (in al), indicating whether a valid target was successfully found and assigned.

**Side effects:**
- Writes a target entity pointer and associated data to the output buffer pointed to by arg_4 if a valid target is located.

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x004068F7, 0x00406B21  
  Called conditionally during state-dependent branching (at 0x004068F7 and 0x00406B21) with the entity pointer (ebx) and an offset/substructure pointer (edi) as arguments, returning a boolean condition (in al) that determines whether the entity transitions states or performs specific actions.

**Calls:**
- `CheckEntityGridFilter` (0x00417E10) - call at 0x004065EE
- `CalculateDistance` (0x00417F80) - call at 0x004065FF
- `_CIsqrt` (0x0041A212) - call at 0x004066F9

### CheckEntityGridFilter - `0x00417E10`

_original name `sub_417E10` | `bool __stdcall CheckEntityGridFilter(void *entityData)` | confidence: medium | The disassembly clearly shows coordinate scaling, float-to-int conversion, clamping to 0x5F (95), and 2D grid index calculations into an array structure, but precise struct layouts are inferred from address arithmetic. | source: llm_

**Converts spatial coordinates into grid/lookup indices via floating-point arithmetic and integer clamping, then checks a bit flag within a lookup table or grid structure.**

The function performs 2D spatial coordinate mapping into a grid or lookup table. It reads floating-point coordinates from the x87 floating-point stack, multiplies them by a scaling constant from .rdata, converts them to integers via __ftol2_sse, and clamps the resulting indices between 0 and 95 (0x5F). It then performs index arithmetic using a multi-dimensional array or stride calculation (multiplying indices by 96/3/etc.) offset by an argument pointer, accesses a byte within that structure at offset +4, and extracts a specific bit using a shift and bitwise AND operation to return a boolean pass/fail status.

**Parameters:**
- `entityData` (void *, stack [esp+8] (arg_0)) - Pointer to entity or grid configuration data structure used as the base for array indexing.

**Returns:** bool in eax (0 or 1, extracted via bitwise operations from the calculated table entry)

**Called by:**
- `FindNearestValidEntityOrTarget` (0x00406540) - call at 0x004065EE  
  Called inside a loop over global entities at address 0x004065EE. Passes a pointer to a global data offset in edx as an argument. The returned boolean in al determines whether the entity passes an initial filter check, allowing the function to proceed to evaluate a distance or geometry check with sub_417F80.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00417E25, 0x00417E60

### CalculateEntityCentroidOrAverage - `0x004062E0`

_original name `sub_4062E0` | `void* __stdcall CalculateEntityCentroidOrAverage(void *entity, void *outPoint)` | confidence: high | Unrolled loops processing two distinct entity collections, floating-point accumulation, and fallback coordinate copying based on count checks. | source: llm_

**Calculates the average spatial position or centroid from an entity's internal collections of points and referenced objects, storing the result in an output structure.**

The function begins by inspecting a count field at offset +0x3C8 of the entity structure. It iterates through an array of items starting at offset +0x24C, accumulating vector or coordinate data. Next, it examines a second collection starting around offset +0x3D0/+0x3D4 with counts at +0x3DC, checking a flag at offset +0x18 of each referenced object (filtering out items where bit 0 of offset 0x18 is set). If any valid points or items are accumulated, it computes an average/centroid by dividing the accumulated totals by the count, writing the resulting vector/coordinates to the output buffer supplied in arg_4. If the total count is zero, it falls back to copying default coordinates from offsets +0x34 and +0x38 of the entity structure.

**Parameters:**
- `entity` (void*, [esp+0Ch+arg_0]) - Pointer to the main entity object containing collections and spatial data.
- `outPoint` (void*, [esp+0Ch+arg_4]) - Pointer to an output buffer or vector structure where the calculated or fallback point is stored.

**Returns:** Returns the output point pointer (arg_4) in eax.

**Side effects:**
- Writes calculated spatial coordinates or fallback values into the memory buffer pointed to by arg_4.

**Data structures:**
- Entity: +0x34 fallback coordinate/value X, +0x38 fallback coordinate/value Y, +0x24C array of points/sub-structures, +0x3C8 count of first collection, +0x3D0/+0x3D4 array of item pointers, +0x3DC count of second collection
- Item: +0x18 flags byte (bit 0 used for filtering)

**Called by:**
- `ProcessEntityCommandBatchOrSpatialQuery` (0x00406C90) - call at 0x00406DCE  
  Called at 0x00406DCE with the entity pointer (`ebp`), a local output buffer (`var_68`), a count (18h), and reference structures (including `sub_40CAA0` and `var_D8`), likely to perform a query, sort, or spatial lookup on the collected items.
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x0040678C  
  Called at 0x0040678C, passing the entity pointer in ebx and a stack variable pointer in ecx, likely to query or initialize entity-specific property/status data.

### CalculateEntityStatsOrAttributes - `0x00405AD0`

_original name `sub_405AD0` | `float __fastcall CalculateEntityStatsOrAttributes(void *entity)` | confidence: medium | Based on register usage (ebx for entity pointer) and extensive floating-point arithmetic across array chunks, matching entity property evaluation routines. | source: llm_

**Performs aggregate mathematical computations on collections of attributes or modifier pointers stored within an entity structure, iterating over arrays in chunks of 4.**

The function evaluates multiple property/attribute arrays from the entity structure passed in `ebx`. It uses unrolled loops processing elements in blocks of 4 using pointer arithmetic, checking validity flags (`[edx+18h]` and `[eax+0Ch]`) on associated sub-objects before performing floating-point calculations (additions, multiplications, and divisions). Finally, it combines the results into an accumulated floating-point value returned via the x87 floating-point stack (`st(0)`).

**Parameters:**
- `entity` (void*, ebx) - Pointer to the entity structure containing attribute lists and counts.

**Returns:** Calculated floating-point statistic or attribute value in st(0).

**Data structures:**
- `Entity`

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x0040677D  
  Called near the beginning of the function (at 0x0040677D) to fetch some initial value or perform a preliminary computation involving global data before setting up entity behavior state checks.

### CUnit::EvaluateAbilitiesAndDispatchCommands - `0x004079D0`

_original name `sub_4079D0` | `void __fastcall CUnit::EvaluateAbilitiesAndDispatchCommands(void *entity, void *edx_unused)` | confidence: high, clear control flow with consistent caller context and well-identified callee semantics | source: llm_

**Iterates over a collection of sub-components or queued capabilities within an entity, validating timer thresholds, target/combat conditions, and rate limits to serialize and dispatch command packets.**

The function begins by checking a count stored at [edi+3DCh]. If greater than zero, it enters a loop iterating over elements starting at offset 0x138 (`edi+138h` + index * 4). For each element, it retrieves a pointer `esi` from offset +0x298. If a flag byte at `[esi+18h]` has bit 0 set (via `not cl; test cl, 1`), it bypasses the timer check and directly calls `Entity_CheckAndSerializeCommand`, `CUnit::EvaluateCombatActionOrAcquireTarget`, and conditionally `EvaluateEntityAbilityCriteriaAndCast`. Otherwise, it enforces a cooldown/rate-limit check by comparing the global tick counter at `[0x42AC60+5BBC0h]` against a timestamp stored in the element. If enough time has passed (>= 30 ticks) and additional coordinate/state thresholds are met, it scans an internal array from `edi+3E0h` of length `[edi+4E0h]` for specific unit criteria before using `DispatchCommandPacket` to send a command and updating the last-action timestamp.

**Parameters:**
- `entity` (void *, edi) - Pointer to the main entity / unit instance.

**Returns:** none

**Side effects:**
- Serializes and dispatches network command packets via DispatchCommandPacket or Entity_CheckAndSerializeCommand
- Updates internal cooldown timestamps within entity/component arrays
- Triggers ability evaluation and target acquisition routines

**Data structures:**
- Entity: +0x138 array of component/ability pointers, +0x3DC count of items, +0x3E0 secondary item array, +0x4E0 secondary item count, +0x5BBC0 global tick counter reference via data

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034B6  
  Called as part of the sequence of sub-routine updates for the current entity when interval and flag checks pass.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00407A4E
- `DispatchCommandPacket` (0x004046B0) - call at 0x00407ADC
- `Entity_CheckAndSerializeCommand` (0x00406EF0) - call at 0x00407AF8
- `CUnit::EvaluateCombatActionOrAcquireTarget` (0x004070A0) - call at 0x00407AFE
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x00407B0B

### EvaluateEntityAbilityCriteriaAndCast - `0x004071C0`

_original name `sub_4071C0` | `bool __stdcall EvaluateEntityAbilityCriteriaAndCast(void *entity, void *component)` | confidence: high | Based on clear interaction with spatial queries, filtering callbacks, distance calculations, and packet dispatching routines. | source: llm_

**Evaluates spatial queries and targeting criteria for an entity's sub-components, checking conditions such as counts of nearby filtered entities or raycast line-of-sight to trigger and dispatch command packets.**

The function performs comprehensive targeting and ability logic for an entity sub-component. First, it executes a spatial grid query using SpatialGridQueryEntities and ValidateEntityCriteriaFilter to populate a local array of nearby entities within range. It then iterates over potential states/conditions stored in the component structure (up to 3 iterations), indexing into criteria sub-tables. Depending on the specific condition type (handled via a switch/jump table), it evaluates various spatial relationships, such as counting entities within range, checking geometric distances via CalculateDistance, verifying visibility/collision using CheckRaySegmentCollisionOrDistance, or checking flags on entity filters. When conditions are met, it constructs and dispatches network command packets using DispatchCommandPacket to execute the ability or interaction.

**Parameters:**
- `entity` (void *, [esp+13Ch+arg_0]) - Pointer to the primary entity being processed.
- `component` (void *, [esp+13Ch+arg_4]) - Pointer to the sub-component structure associated with the entity.

**Returns:** bool (in al/eax, though typically called as a subroutine returning via retn 8)

**Side effects:**
- Queries the spatial grid and filters entities.
- Sends outgoing network command packets via DispatchCommandPacket.

**Called by:**
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x00407B0B  
  Called conditionally (if byte [esi+70h] != 0Dh) after sub_4070A0 during sub-component processing, passing the entity pointer and component pointer as arguments.

**Calls:**
- `ValidateEntityCriteriaFilter` (0x0040C950) - address taken at 0x00407205, 0x004074E1
- `SpatialGridQueryEntities` (0x004177F0) - call at 0x0040722F, 0x0040750F
- `CalculateDistance` (0x00417F80) - call at 0x00407372, 0x0040747E, 0x004077B8, 0x004077EF
- `DispatchCommandPacket` (0x004046B0) - call at 0x0040742E, 0x00407737, 0x00407976
- `Vec3_NormalizeOrZero` (0x00417FD0) - call at 0x004078A9
- `CheckRaySegmentCollisionOrDistance` (0x004183C0) - call at 0x00407902

### CheckRaySegmentCollisionOrDistance - `0x004183C0`

_original name `sub_4183C0` | `bool __fastcall CheckRaySegmentCollisionOrDistance(void *this, void *edx_param)` | confidence: medium, control flow and callee purpose are clear, but exact floating-point vector semantics are inferred from the math pattern | source: llm_

**Performs a 2D geometric test by calling a ray/segment intersection routine and evaluating vector dot products or squared distances to determine a boolean outcome (such as line-of-sight obstruction or distance threshold validation).**

The function allocates local stack space and calls sub_4182F0 (IntersectRaySegmentOrLine), passing a pointer to a temporary coordinate buffer via edx. Following the call, it performs a sequence of x87 floating-point arithmetic operations, computing differences, products, and sums (characteristic of vector dot products, magnitudesquared calculations, or squared distance comparisons). It checks the resulting flags using FPU status word extraction (`fnstsw ax` and `test ah, 41h`) to evaluate a conditional relation (such as a distance threshold check or intersection success/failure), returning 1 if the condition is met and 0 otherwise.

**Parameters:**
- `this` (void*, ecx) - Object context pointer
- `edx_param` (void*, edx) - Pointer to geometric data or parameters passed to the intersection routine

**Returns:** bool (1 or 0) in eax

**Called by:**
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x00407902  
  Called in a loop to perform line-of-sight, collision, or accessibility checks between a source position/entity and multiple candidate targets.

**Calls:**
- `IntersectRaySegmentOrLine` (0x004182F0) - call at 0x004183CA

### SpatialGridQueryEntities - `0x004177F0`

_original name `sub_4177F0` | `int __stdcall SpatialGridQueryEntities(void *grid_data, float center_x, float center_y, float radius, void *callback_filter, void *user_data, void **out_entity_array, int max_entities)` | confidence: high, clear structure mapping directly to spatial grid iteration, distance checks, callback filtering, and buffer population. | source: llm_

**Queries a spatial grid structure for entities within a bounding box or radius, filters them using distance checks and an optional callback, and populates an output array.**

The function begins by calling CalculateGridBounds using input floating-point coordinates/ranges to determine the grid cell range indices (min/max x and y). It then iterates over the 2D grid cells within these bounds. For each grid cell, it traverses a linked list of entities starting at the cell's head pointer (offset 0x0). For each entity, it invokes a virtual method at offset +0x18 to retrieve position data, performs floating-point distance calculations against the query center and radius, and checks if the entity meets the distance criteria. If a callback function pointer is provided, it invokes the callback with the entity and user data; if the callback returns true, the entity pointer is stored into the output array. Iteration continues until the maximum entity count is reached or all candidate grid cells and entities have been processed.

**Parameters:**
- `grid_data` (void*, [esp+30h]) - Pointer to the spatial grid data structure or manager.
- `center_x` (float, st(0) / stack) - X coordinate of the query center.
- `center_y` (float, stack) - Y coordinate of the query center.
- `radius` (float, stack) - Search radius or distance limit.
- `callback_filter` (int (__cdecl*)(void*, void*), stack) - Optional filter callback function returning non-zero to accept an entity.
- `user_data` (void*, stack) - User-defined context passed to the callback.
- `out_entity_array` (void**, stack) - Output buffer to store matching entity pointers.
- `max_entities` (int, stack) - Maximum number of entities the output array can hold.

**Returns:** Number of matching entities found and stored (in eax).

**Side effects:**
- Writes matching entity pointers into the provided output array.

**Data structures:**
- Entity: +0x18 virtual method for position/transform retrieval, +0x240 next pointer in linked list

**Called by:**
- `ProcessEntityCommandBatchOrSpatialQuery` (0x00406C90) - call at 0x00406DE4  
  Called at 0x00406DE4 to retrieve a count or limit (`esi`), passing a global configuration/manager pointer from `dword_42AC60 + 43D7Ch` and the return value or result from the previous lookup routine (`sub_4062E0`).
- `CalculateEntityProximityValue` (0x00406000) - call at 0x00406055  
  Called with a query configuration struct (size 0x30, flags set to 0x101), an entity pointer, a spatial/distance limit, and the callback function pointer sub_40C950 to perform a spatial search or proximity query, returning the count of matching entities in eax.
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x0040722F, 0x0040750F  
  Called to perform spatial queries or find entities within a range or area around a center point, populating a list/array of candidate objects for targeting evaluation.

**Calls:**
- `CalculateGridBounds` (0x004174B0) - call at 0x00417816

### CalculateGridBounds - `0x004174B0`

_original name `sub_4174B0` | `void __stdcall CalculateGridBounds(float min_val, float max_val, float *out_min_idx, float *out_max_idx)` | confidence: high, clear repetitive pattern of floating-point range clamping to a fixed grid size of 24 (0x17) bins/cells. | source: llm_

**Calculates and clamps spatial grid or bounding box indices from floating-point coordinate ranges, storing the resulting integer indices into output pointers.**

The function processes spatial coordinate bounds using x87 floating-point arithmetic. It normalizes and converts floating-point ranges into integer grid indices clamped between 0 and 23 (0x17), repeating this clamping and conversion logic four consecutive times for different components/axes. Each iteration converts a float value to an integer using __ftol2_sse, applies rounding adjustment logic via FPU comparisons, clamps the resulting integer to the range [0, 23], and writes the resulting coordinate index into corresponding output pointer parameters.

**Parameters:**
- `min_val` (float, st(0) / stack) - Minimum spatial boundary value
- `max_val` (float, stack) - Maximum spatial boundary value
- `out_min_idx` (int *, stack) - Pointer to receive the lower clamped grid index
- `out_max_idx` (int *, stack) - Pointer to receive the upper clamped grid index

**Returns:** none

**Side effects:**
- Writes clamped integer indices to memory addresses provided via output pointers.

**Called by:**
- `SpatialGridQueryEntities` (0x004177F0) - call at 0x00417816  
  Called once at the beginning of the function, passing pointers to several local stack variables to initialize or retrieve spatial grid / bounding range bounds or indexing parameters.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x004174D0, 0x00417514, 0x00417557, 0x00417599

### ValidateEntityCriteriaFilter - `0x0040C950`

_original name `sub_40C950` | `bool __fastcall ValidateEntityCriteriaFilter(const void *entity, const void *queryCriteria)` | confidence: high, clear control flow structure with well-defined predicate checks and sub-routine calls | source: llm_

**Evaluates whether a given entity matches a comprehensive set of filtering criteria, flags, type constraints, and spatial or status conditions specified in a query criteria structure.**

The function performs a multi-stage validation check on an entity (`entity`, passed as `edi`). First, it checks basic status flags at `[entity+18h]`, immediately rejecting the entity if specific dead/inactive flags are set. Then, it inspects a criteria structure (`queryCriteria`, passed as `ebp`). If flag `0x200` is set in `queryCriteria+8`, it excludes the entity if it matches a specific self-reference pointer (`queryCriteria+0`). Next, it evaluates various bit flags in the criteria bitfield (`ebx`, located at `queryCriteria+8`): bit 0 checks entity type matching via sub_4042B0; bit 1 verifies entity category bits against a bitmask in a global table; bits 4, 5, and 6 inspect sub-status bytes at `[entity+1C]` for specific flag combinations. Further bit checks handle floating-point comparison ranges and condition evaluations. Finally, if bit `0x100` is set, it invokes sub_403320 (`CheckEntityCriteriaMatch`) for advanced criteria evaluation. If all applicable checks pass, it returns true (`al = 1`); otherwise, it aborts early and returns false (`al = 0`).

**Parameters:**
- `entity` (const void *, edi (or stack/arg_0)) - Pointer to the entity being evaluated against the filter criteria.
- `queryCriteria` (const void *, ebp (or stack/arg_4)) - Pointer to a criteria/query parameter structure containing bit flags, offsets, and limits.

**Returns:** bool (1 if all criteria match, 0 otherwise), returned in al

**Data structures:**
- Entity: +0x18 int statusFlags, +0x1B byte entityType, +0x1C void *subStruct
- QueryCriteria: +0x0 void *targetEntity, +0x4 int paramOrMultiplier, +0x8 unsigned int criteriaFlags

**Called by:**
- `ValidateAdvancedEntityMatch` (0x0040CAA0) - call at 0x0040CAAC  
  Called at the beginning of the function, passing the two input arguments (`arg_0` as `esi` and `arg_4` as `edi`) to perform an initial preliminary check or compatibility test between them. If the return value (`al`) is zero, the function immediately aborts and returns false.
- `CalculateEntityProximityValue` (0x00406000) - address taken at 0x00406029  
  Passed as a callback function pointer (along with stack space for results and search parameters) to sub_4177F0, likely serving as a predicate or collector callback invoked during spatial range-query or collision/proximity checks.
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - address taken at 0x00407205, 0x004074E1  
  Passed as a callback function pointer in spatial query/filtering calls (along with entity and search parameters) to test or filter candidate entities.

**Calls:**
- `CheckEntityTypeMatch` (0x004042B0) - call at 0x0040C9A5
- `CheckEntityCriteriaMatch` (0x00403320) - call at 0x0040CA7E

### CheckEntityCriteriaMatch - `0x00403320`

_original name `sub_403320` | `bool __fastcall CheckEntityCriteriaMatch(const void *entityPtr, int param)` | confidence: high based on clear register usage, caller expectations, and straightforward branching logic | source: llm_

**Evaluates a specific filter criterion on an entity by inspecting its status flags, type attributes, and either checking a bitfield or falling back to a grid lookup function.**

The function begins by testing bit 6 (0x40) of a byte at offset +0x18 within the entity structure (`edx`). If this bit is set, it checks if a byte at offset +0x1B (`cl`) is <= 5; if out of range, it returns false (0). Otherwise, it multiplies `ebx` (the parameter passed in `eax`) by a global stride or base value stored at `0x42AC60`, computes a bitmask via `1 << cl`, and tests this bit against an array/table located at `[ebx + eax + 0xE8]`, returning true if set. If bit 6 of `[edx+18h]` is clear, it first checks if `ebx < 4`. If so, it performs a similar bitfield check; if that check succeeds, it returns true. If the check fails or `ebx >= 4`, it adds an offset to the base lookup value, points to `edx+8`, and delegates to `EvaluateGridLookup` (`sub_417D70`), returning its result.

**Parameters:**
- `entityPtr` (const void *, edx) - Pointer to the entity or component data structure being evaluated.
- `param` (int, eax) - An integer scalar or index parameter used in stride calculations and bitfield lookups.

**Returns:** bool in al (1 for match/success, 0 for rejection)

**Data structures:**
- `Entity`

**Called by:**
- `ValidateEntityCriteriaFilter` (0x0040C950) - call at 0x0040CA7E  
  Called conditionally when bit 8 (`0x100`) of the criteria flags (`ebx`) is set, passing the entity pointer in `edx` (`edi`) and a scalar/parameter in `eax` (`ebp+4`), using the boolean return value in `al` to determine whether to accept or reject the entity.

**Calls:**
- `EvaluateGridLookup` (0x00417D70) - call at 0x0040338B

### EvaluateGridLookup - `0x00417D70`

_original name `sub_417D70` | `int __cdecl EvaluateGridLookup(const void *dataPtr)` | confidence: medium, floating-point index calculation and clamping logic are clear, but exact struct definitions for the underlying table layout are inferred from index math. | source: llm_

**Performs a 2D grid or lookup-table index calculation using floating-point coordinates and bounds clamping, then checks a bit within a data structure.**

The function takes a pointer argument and uses floating-point calculations involving scaling factors to compute indices. It converts floating-point values to integers using __ftol2_sse, clamps the resulting integer values to the range [0, 95] (0x5F), and uses them to compute a nested array/grid offset (`eax = (((ebx*3) << 5) + ecx) * 96 + esi`). Finally, it retrieves a byte from the computed address offset plus four, masks it with 1, and returns the result as an integer boolean.

**Parameters:**
- `dataPtr` (const void *, [esp+8]) - Base data pointer passed from the caller, used as the base for the grid/lookup array.

**Returns:** int in eax (0 or 1, representing the extracted bit flag)

**Called by:**
- `CheckEntityCriteriaMatch` (0x00403320) - call at 0x0040338B  
  Called as a fallback query/validation method when the primary entity bitmask check fails or when specific numeric threshold criteria are met (ebx < 4 check fails and initial bit test yields zero). Passes a modified data pointer/offset (derived from the global data and an added constant 0x4467C) as an argument on the stack, while `edi` points to an offset (`edx+8`) within the entity structure.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00417D85, 0x00417DC0

### CheckEntityTypeMatch - `0x004042B0`

_original name `sub_4042B0` | `bool __fastcall CheckEntityTypeMatch(const void *entity, unsigned int entityType)` | confidence: high | complete control flow and data structure references understood from code and caller context | source: llm_

**Evaluates whether a given entity matches a target entity type or bitmask category, returning true if it matches and false otherwise.**

The function performs a series of validation and type-matching checks using the entity pointer passed in `eax` and the target type/category ID passed in `ecx`. First, it compares the byte at `[eax]` with `ecx`; if they are equal, it immediately returns true (`loc_4042DE` falls through to a success path or early exit sequence). It then checks if `ecx` is greater than 5, or if bit 4 (value 0x10) is set at offset `[eax+3]`, or if `ecx` equals 5; if any of these conditions are met, it rejects the entity by returning false (`xor al, al`). Otherwise, it retrieves a bitfield from offset `+0xE8` of the entity structure (`[eax+0E8h]`), tests it against a bitmask generated by shifting 1 left by `ecx` (`1 << cl`), and normalizes the resulting bitwise AND operation into a boolean 0 or 1 return value.

**Parameters:**
- `entity` (const void*, eax) - Pointer to the entity or component structure being evaluated.
- `entityType` (unsigned int, ecx) - Target entity type identifier or category bit index.

**Returns:** bool in al (1 if the entity matches the criteria, 0 otherwise)

**Data structures:**
- Entity: +0x0 byte type/category, +0x3 byte flags (bit 4 used for exclusion), +0xE8 unsigned int type bitmask/flags

**Called by:**
- `ValidateEntityCriteriaFilter` (0x0040C950) - call at 0x0040C9A5  
  Called conditionally when the first criteria flag (`bl & 1`) is set, passing a derived/indexed entity property in `eax` (from `esi`) and an entity type byte in `ecx` (from `edi+1Bh`), and aborts/rejects the entity if the returned boolean `al` is zero.

### CUnit::EvaluateCombatActionOrAcquireTarget - `0x004070A0`

_original name `sub_4070A0` | `void __fastcall CUnit::EvaluateCombatActionOrAcquireTarget(void *entity, void *edx_unused, int arg0)` | confidence: high, based on clean structure, standard iterator loops over fixed-size arrays, and clear integration with known command packet and entity search functions. | source: llm_

**Evaluates unit combat states, checks proximity/range to targets or entities in an internal array, and dispatches command packets if criteria are met.**

The function begins by loading several constant floating-point values and setting up pointers, referencing an internal array at `esi+264h` containing up to 6 elements. It loops through this array checking the entity type byte (`+1Ch`) against specific values (0, 1, and 0Ah). For matching types, it performs distance or range comparisons using floating-point math. If a condition matches and a flag at `esi+1F4h` is set, it cleans up the FPU stack and dispatches a command packet using `DispatchCommandPacket` (sub_4046B0). If no array element triggers a match, it checks alternative state conditions and calls `FindNearestValidEntityInRange` (sub_4030C0) to find a target within range. If a valid target is located and passes type lookup checks, a secondary command packet is dispatched via `DispatchCommandPacket` to engage or interact with it.

**Parameters:**
- `entity` (void*, esi) - Pointer to the main entity object.
- `arg0` (int, [esp+4]) - Parameter passed along to DispatchCommandPacket.

**Returns:** none

**Side effects:**
- Dispatches network command packets using DispatchCommandPacket

**Data structures:**
- Entity: +0x8 position/coords, +0x70 state byte, +0x1F4 action flag, +0x264 internal entity collection/array

**Called by:**
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x00407AFE  
  Called immediately after sub_406EF0 during the alternative branch for an invalid or inactive sub-component state, passing the main entity pointer (edi).

**Calls:**
- `DispatchCommandPacket` (0x004046B0) - call at 0x00407108, 0x004071B3
- `FindNearestValidEntityInRange` (0x004030C0) - call at 0x0040715B

### FindNearestValidEntityInRange - `0x004030C0`

_original name `sub_4030C0` | `void* __stdcall FindNearestValidEntityInRange(float x, float y, float max_distance)` | confidence: high, clear iteration pattern over entity lists with coordinate distance checks and bitwise flag filtering. | source: llm_

**Iterates through a global list of entities/objects, filtering out inactive, dead, or invalid items and finding one that satisfies specific state flags and lies within a given distance threshold.**

The function accesses a global manager/array structure located via `dword_42AC60`, reading a pointer list and count (`+0x573A4` and `+0x573A8`). It unrolls an inner loop processing entities 4 at a time, followed by a cleanup loop for any remaining items. For each entity pointer encountered, it checks status flags at offset `+0x18` (verifying bit 0 is cleared/active, bit 1 is not set) and ensures a state/pointer at offset `+0x20` is zero. It computes a distance squared metric using floating-point operations against input coordinates, comparing it to a threshold limit, and returns the pointer to the first matching entity or `NULL` if none qualify.

**Parameters:**
- `x` (float, st(0) / stack (arg_0)) - X coordinate for distance checking
- `y` (float, stack (arg_4)) - Y coordinate for distance checking

**Returns:** Pointer to the matching entity object (in eax), or NULL (0) if none found.

**Data structures:**
- Entity object: +0x18 flags byte (bit 0: active/enabled, bit 1: filtered/dead), +0x20 target/state pointer

**Called by:**
- `CUnit::EvaluateCombatActionOrAcquireTarget` (0x004070A0) - call at 0x0040715B  
  Called with a pointer derived from esi ([esi+8]) to retrieve a related entity, component, or target pointer, whose validity and type properties are then checked before initiating further action.

### Entity_CheckAndSerializeCommand - `0x00406EF0`

_original name `sub_406EF0` | `__thiscall void Entity_CheckAndSerializeCommand(void *entity, int arg1, int arg2)` | confidence: high, clear register usage, standard network serialization pattern matching known engine routines | source: llm_

**Validates entity state/timers and serializes a network command packet into a bitstream if rate limits and conditions are satisfied.**

The function first checks if the entity pointer's field at offset +0x258 is positive. It then verifies if enough time/ticks have elapsed since the last command for this index (using a global tick tracker at 0x42AC60 and an array offset at entity+arg_4*4+0x12C). Next, it performs validation checks on entity byte fields (offsets +0x260 to +0x262) and lookup tables to ensure the entity is in a valid state for command transmission. If checks pass, it constructs a command packet structure on the stack (including entity coordinates from offsets +4, +8, +0xC and other parameters) and either updates counter statistics or calls sub_418AA0 (BitStream_WriteBits) and sub_4087D0 (SerializeCommandPacket) to serialize the command into a network bitstream, updating the timestamp array for the given index.

**Parameters:**
- `entity` (void*, eax (implied thiscall / register convention)) - Pointer to the entity object being checked.
- `arg_0` (void*, [esp+0x54]) - Pointer to a context or packet destination structure.
- `arg_4` (int, [esp+0x58]) - Index or identifier used for timing and array lookups.

**Returns:** none

**Side effects:**
- Writes to global timer/packet statistics
- Serializes data into a bitstream via sub_4087D0 and sub_418AA0
- Updates entity-related timestamp/state arrays

**Data structures:**
- `Entity`

**Called by:**
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x00407AF8  
  Called during sub-component validation checks when an internal flag test fails (specifically when bit 0 of byte [esi+18h] is set), passing the entity pointer and a component-related pointer to perform cleanup or state resetting.

**Calls:**
- `BitStream_WriteBits` (0x00418AA0) - call at 0x00407062
- `SerializeCommandPacket` (0x004087D0) - call at 0x0040706E

### Entity::ProcessTimedBehaviorsOrActions - `0x00405040`

_original name `sub_405040` | `bool __fastcall Entity::ProcessTimedBehaviorsOrActions(void *pEntity)` | confidence: high | well-structured control flow matching entity update subsystems | source: llm_

**Iterates through a configuration-driven sequence of state actions or behavior frames for an entity, using Entity::ProcessStateTransitionOrAction, and updates timing counters or loop indices upon success or special termination conditions.**

The function begins by computing a table offset using the entity's data at `[ebp+0]` multiplied by itself, offset by a global value `d_42AC60`, to select between two configuration tables in `.rdata` (`unk_41CCC0` or `unk_41CD70`). It determines a loop count (`edi`) and a limit or secondary threshold based on configuration bytes. It then enters a loop running up to `edi` iterations. In each iteration, it fetches an entry from the configuration array, conditionally checks command/action attributes using calculated offsets from `0x41B178`, and invokes `Entity::ProcessStateTransitionOrAction` (`sub_404EA0`). If the action handler returns true, it advances to the next entry (`esi++`, `ebx += 4`). If a specific configuration byte indicates a special condition (value 6) and an internal counter (`[ebp+8]`) is less than 0x14, it increments that counter and exits early. Otherwise, it updates the entity's state progress counter at `[ebp+4]` and returns whether all actions succeeded or the sequence completed.

**Parameters:**
- `pEntity` (void *, [esp+8]) - Pointer to the entity structure being processed.

**Returns:** bool in al / eax (true if processing completed successfully or advanced, false otherwise)

**Side effects:**
- Modifies entity fields at offset +0x4 and +0x8 (progress counters/timers).
- Invokes Entity::ProcessStateTransitionOrAction which updates entity state and flags.

**Data structures:**
- `Entity`

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034B1  
  Called with `edi` (the current entity structure) pushed on the stack as an argument, within the conditional entity update sequence.

**Calls:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x004050CE

### Entity::ProcessStateTransitionOrAction - `0x00404EA0`

_original name `sub_404EA0` | `bool __fastcall Entity::ProcessStateTransitionOrAction(void *pEntity, void *edx_unused, const unsigned char *pConfigData)` | confidence: high | switch-case state dispatcher well-structured with clear helper function associations. | source: llm_

**Evaluates a configuration command/action byte array against entity state attributes and dispatches corresponding actions or updates state flags.**

The function reads a configuration byte pointer from `ebx` and an entity pointer from `eax`. It extracts a command/operation type byte via `movzx eax, byte ptr [ebx]`, multiplies it with an offset from the entity's base structure, and uses a switch-case jump table (indices 0 through 5) to handle different entity operations. Depending on the case, it validates conditions like entity counts, placement criteria, cooldown limits, and capacity caps using helper functions (`CheckEntityConditionsOrCount`, `CheckEntityPlacementOrActionCriteria`, `ValidateCapacityOrLimit`), and executes abilities or actions (`TryPerformActionOrCommand`, `CheckAndExecuteUnitAbility`, `CheckAndDispatchAction`). Case 6 directly sets a state byte at offset +0x3C on the entity and returns true; all other action branches return false (`xor al, al`) upon completion.

**Parameters:**
- `pEntity` (void*, eax) - Pointer to the entity instance being updated.
- `pConfigData` (const unsigned char*, ebx) - Pointer to the configuration data bytes defining the action/condition type and parameters.

**Returns:** Returns a boolean value in al (1 for success/continuation on specific state updates, 0 otherwise).

**Side effects:**
- Invokes various action and ability execution routines which may dispatch command packets, update timestamps, or modify entity states.
- Modifies entity byte field at offset +0x3C in the default state transition branch.

**Data structures:**
- Entity: +0x0 pointer to base/sub-structure, +0x10 limit/capacity field, +0x3C state byte, +0x76/+0x14 word attributes, +0xF4 count/limit field

**Called by:**
- `Entity::ProcessTimedBehaviorsOrActions` (0x00405040) - call at 0x004050CE  
  Invoked in a loop over entity configuration items (up to an index limit determined by local flags and count structures) with the entity pointer in `eax` and a calculated parameter pushed on the stack. The return value in `al` dictates whether the loop continues to the next iteration or terminates early.

**Calls:**
- `CheckEntityConditionsOrCount` (0x00404DF0) - call at 0x00404ECF, 0x00404F0A
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x00404EF4, 0x00404F48
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x00404F35
- `GetConfigTableEntry` (0x00417EB0) - call at 0x00404F80
- `ValidateCapacityOrLimit` (0x00404260) - call at 0x00404F8C
- `CheckAndExecuteUnitAbility` (0x00405110) - call at 0x00404F97, 0x00404FC9
- `CheckAndDispatchAction` (0x00405230) - call at 0x00404FFD

### CheckAndDispatchAction - `0x00405230`

_original name `sub_405230` | `char __fastcall CheckAndDispatchAction(void *entity, void *edx_unused, int actionIndex)` | confidence: high | standard reverse engineering based on register usage, memory offsets, and callee integration | source: llm_

**Validates command cooldown timers, resource requirements, and target state for a specific action index, returning false if conditions fail or dispatching a command packet via DispatchCommandPacket and updating the cooldown timestamp on success.**

The function first checks a global tick/time counter (from `dword_42AC60`) against a cooldown timestamp stored in the entity structure at `[edi + esi*4 + 10Ch]`. If less than 30 ticks (1Eh) have elapsed, it returns `0` (failure). Otherwise, it evaluates table-driven resource or cost requirements using configuration data from `.rdata` (`0x41CB18`, indexed by `esi`), verifying them against values in the entity structure. It then iterates through an array of objects (`[edi + 3E0h]`, count at `[edi + 4E0h]`) to find a valid target matching specific flags and state criteria (`[edx + 5Ch]`, bit `0x10` in `[edx + 18h]`, and state `>= 2` at `[edx + 29Ch]`). If a valid target is found, it calls `DispatchCommandPacket` to transmit the action packet, updates the cooldown timestamp in the entity for the action index, and returns `1` (success). If any check fails or no valid target is found, it aborts and returns `0`.

**Parameters:**
- `entity` (void*, edi) - Pointer to the entity or object state structure.
- `actionIndex` (int, esi) - The index of the command/action being validated and executed.

**Returns:** char (1 for success/dispatched, 0 for failure), returned in al

**Side effects:**
- Updates the action cooldown timestamp in the entity structure at [edi + esi*4 + 10Ch] on successful dispatch
- Sends a network command packet by calling DispatchCommandPacket

**Data structures:**
- Entity: +0x10Ch action cooldown timestamps array (4 bytes per index), +0x3E0h pointer/array of target objects, +0x4E0h target count

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404FFD  
  Called to perform a final action or state modification for a specific command case, with the index parameter prepared in esi.

**Calls:**
- `DispatchCommandPacket` (0x004046B0) - call at 0x004052E2

### GetConfigTableEntry - `0x00417EB0`

_original name `sub_417EB0` | `const void* __cdecl GetConfigTableEntry(int index)` | confidence: high, straightforward arithmetic lookup table function with bounds checking | source: llm_

**Retrieves a pointer to a configuration structure or entry from a static table in .rdata based on an input index.**

The function checks if the input index passed in eax is greater than 0x30 (48 decimal). If the index is out of bounds (> 0x30), it falls back to returning the base pointer of the table located at 0x41B178. If the index is within bounds, it computes an offset into the table by scaling the index (effectively calculating index * 15 * 4, since `ecx = eax * 16 - eax = eax * 15`, and then addressing `0x41B178 + ecx * 4`), and returns the resulting table entry pointer in eax.

**Parameters:**
- `index` (int, eax) - Index of the configuration entry to retrieve

**Returns:** Pointer to the selected configuration table entry in eax

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404F80  
  Called with an index in eax to retrieve an associated sub-object or configuration structure pointer, whose properties are then checked against additional constraints.

### Entity::ProcessAIOrProductionAction - `0x00404D20`

_original name `sub_404D20` | `bool __fastcall Entity::ProcessAIOrProductionAction(void *pEntity, void *edx_unused)` | confidence: high | standard reverse engineering of game entity logic involving tables and thresholds | source: llm_

**Evaluates entity-specific production or action progress and criteria, and potentially triggers a command via Entity::TryPerformActionOrCommand.**

The function performs a complex set of calculations to determine whether an entity should execute a specific production or behavior action. It first indexes into a configuration structure using entity state data and global tables (such as at 0x0042AC60 and 0x0041B178). It loops over an array of sub-elements at offset 0x144 of the entity structure, counting items matching certain criteria. It then combines these counts and properties with thresholds derived from entity attributes (at offsets 0x244, 0x4E0) and table data. If the calculated resource or progress value meets specific conditions (falling below a threshold and having fewer than 2 matches), it invokes sub_405300 [Entity::TryPerformActionOrCommand] with an action index to attempt execution.

**Parameters:**
- `pEntity` (void*, eax) - Pointer to the entity structure being processed.

**Returns:** none

**Side effects:**
- Calls Entity::TryPerformActionOrCommand which may dispatch command packets or update action cooldowns.

**Data structures:**
- Entity: +0x0 int type/id, +0x144 sub-elements array, +0x244 int count/limit, +0x4E0 int progress/attribute

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034AB  
  Called with `eax` set to `edi` (the current entity structure) as part of the sequence of update routines for the filtered entity.

**Calls:**
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x00404DE1

### Entity::ProcessCombatOrAbilityUpdate - `0x00404AD0`

_original name `sub_404AD0` | `void __fastcall Entity::ProcessCombatOrAbilityUpdate(void *pEntity)` | confidence: high, based on clear pattern of entity iteration, geometric distance checks, and dispatching command/ability routines | source: llm_

**Evaluates combat proximity and ability conditions for an entity, dispatching targeted command packets or triggering unit abilities and actions based on distance metrics and state criteria.**

The function begins by computing a global offset using the entity's initial dword value and a data table at 0x0042AC60. It then enters a loop governed by the count at [edi+244h], iterating through sub-elements at [edi+144h]. Within the loop, it uses FindNearestValidEntityInRadius to locate targets, computes 2D Euclidean distances across an array of coordinates at [edi+4E4h], and compares them against threshold constants using square root calculations (_CIsqrt). If valid targets are found within range and entity state constraints are met (such as entity flags and sub-element status fields like offset +2C4h), it dispatches network command packets via DispatchCommandPacket. If the loop completes or no targets match, it falls back to broader entity range checks using FindEntityInRange and executes conditional ability checks or tries to perform higher-level actions via CheckAndExecuteUnitAbility, CheckEntityConditionsOrCount, and Entity::TryPerformActionOrCommand.

**Parameters:**
- `pEntity` (void*, eax) - Pointer to the main entity or simulation object being updated.

**Returns:** none

**Side effects:**
- dispatches network command packets using DispatchCommandPacket
- modifies entity combat state and ability cooldown counters
- triggers unit abilities and actions via CheckAndExecuteUnitAbility and TryPerformActionOrCommand

**Data structures:**
- Entity: +0x4 int state/flags, +0x34 position/range vector, +0x144 sub-element array/pointer, +0x244 sub-element count, +0x4E4 coordinate data array, +0x4F4 count/index field

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x004034A4  
  Called immediately after sub_404940 within the same conditional entity update block.

**Calls:**
- `FindNearestValidEntityInRadius` (0x00402E50) - call at 0x00404B3A, 0x00404CC8
- `_CIsqrt` (0x0041A212) - call at 0x00404BA1
- `DispatchCommandPacket` (0x004046B0) - call at 0x00404BFB, 0x00404C48
- `FindEntityInRange` (0x00402A30) - call at 0x00404C1F
- `CheckAndExecuteUnitAbility` (0x00405110) - call at 0x00404CB5
- `CheckEntityConditionsOrCount` (0x00404DF0) - call at 0x00404D00
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x00404D10

### Entity::TryPerformActionOrCommand - `0x00405300`

_original name `sub_405300` | `bool __fastcall Entity::TryPerformActionOrCommand(void *pEntity, void *edx_unused, int actionIndex)` | confidence: high | clean disassembler output with well-defined callee signatures and caller contexts. | source: llm_

**Validates timing cooldowns, resource/structure criteria, geometric placement, and target availability for an entity action, and if successful, dispatches a command packet and updates the action cooldown timestamp.**

The function begins by checking a global time counter retrieved from a global structure against a timestamp stored in the entity structure at [edi + esi*4 + 0x48], ensuring a cooldown/delay of at least 30 ticks has elapsed. It then looks up an action/type configuration table from .rdata based on the action index (esi), and verifies specific requirements such as resource quantities or inventory counts against fields in a resolved sub-structure. Next, it calls CheckEntityPlacementOrActionCriteria to validate spatial/grid conditions and Entity::FindTargetOrValidSubEntity to locate a valid target. If all validation steps succeed, it constructs and dispatches a command packet using DispatchCommandPacket, updates the entity's action timestamp with the current global time, and returns true (1). If any check fails, it immediately aborts and returns false (0).

**Parameters:**
- `pEntity` (void*, edi) - Pointer to the main entity object being evaluated.
- `actionIndex` (int, esi) - The index of the action, command, or capability being evaluated.
- `arg_0` (int, stack [esp+0x10]) - An additional parameter/flag passed to CheckEntityPlacementOrActionCriteria.

**Returns:** Returns true (1) in al if the action was successfully executed and dispatched, or false (0) if any validation check failed.

**Side effects:**
- Dispatches an outgoing network command packet via DispatchCommandPacket.
- Updates the action cooldown timestamp stored at [edi + esi*4 + 0x48].

**Data structures:**
- Entity: +0x0 pointer to internal structure, +0x4/0x8 resource/requirement fields, +0x48+esi*4 action cooldown/timestamp array.

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404EF4, 0x00404F48  
  Called to update or trigger a state change on the entity/system, passing an index (from ebx+1) and a boolean/flag constant (0 or 1) as arguments.
- `Entity::ProcessAIOrProductionAction` (0x00404D20) - call at 0x00404DE1  
  Invoked as part of a conditional trigger when the entity's computed metric falls below a maximum threshold and count criteria are met (esi < 2). It is called with a constant 0 pushed on the stack, while esi is set to the computed classification value (ebx) previously saved during entity evaluation.
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404D10  
  Called at 0x00404D10 as part of a final conditional branch after sub_404DF0, passing an integer constant '1' as an argument.

**Calls:**
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x0040537F
- `Entity::FindTargetOrValidSubEntity` (0x004053D0) - call at 0x0040538F
- `DispatchCommandPacket` (0x004046B0) - call at 0x004053A6

### Entity::FindTargetOrValidSubEntity - `0x004053D0`

_original name `sub_4053D0` | `void* __fastcall Entity::FindTargetOrValidSubEntity(void *this, void *edx, float threshold)` | confidence: medium | Control flow and field access patterns strongly suggest entity sub-component iteration and geometric distance validation. | source: llm_

**Iterates through an entity's sub-components or list of associated elements, performs distance and state validation checks, and returns the first matching element that satisfies the criteria.**

The function operates on an entity object passed in ecx, checking a count at offset +0x244. If the count is greater than zero, it iterates through an array of pointers starting at offset +0x144. For each element, it checks bit 9 of the flags at offset +0x18 and ensures the byte at offset +0x70 is not 0x0A. It computes a 2D or 3D Euclidean distance from the element using coordinates and calls _CIsqrt to obtain the final distance, comparing it against a threshold passed on the stack. It also resolves a resource or entity reference from a global manager table using a packed ID at offset +0x74, applies conditional adjustments based on entity type flags (such as specific checks when offset +0x70 equals 8 or 0), and validates that the computed distance falls within an acceptable range before marking the element as valid in a local variable and terminating the search early.

**Parameters:**
- `this` (void*, ecx) - Pointer to the entity object.
- `threshold` (float, [esp+4] / arg_0) - Distance or parameter threshold used for validation comparisons.

**Returns:** Pointer to the matched sub-entity or element in eax, or NULL (0) if no match is found.

**Data structures:**
- `Entity`
- `SubElement`

**Called by:**
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x0040538F  
  Called with the entity pointer (edi) and a local work variable reference (var_8) to perform a secondary validation step or resource lookup, returning an object or context pointer in eax which must be non-zero to proceed.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00405458

### CheckEntityPlacementOrActionCriteria - `0x00405520`

_original name `sub_405520` | `bool __stdcall CheckEntityPlacementOrActionCriteria(void *pEntity, int arg_4, Vector2 *outPos)` | confidence: high | complete disassembly and caller context match spatial/grid validation routine | source: llm_

**Evaluates complex geometric, proximity, and grid placement/action criteria for an entity, checking surrounding entities and map grid regions before validating target coordinates.**

The function begins by computing configuration parameters based on an input value/index from the entity state. It calls FindNearestValidEntityInRadius to locate the closest entity within a specified range. It then iterates through global entity lists and grid areas, testing validity flags, calculating Euclidean distances using _CIsqrt, and verifying grid cell passability criteria via CheckGridAction (sub_414CE0). Geometric projections and ray/segment intersections (via IntersectRaySegmentOrLine) are performed to test line-of-sight or boundary clearance. If all spatial, grid, and proximity conditions are satisfied, the resulting target 2D coordinates are written to the output buffer pointer passed as argument 3, and true is returned; otherwise, false (al == 0) is returned.

**Parameters:**
- `pEntity` (void*, [ebp+8]) - Pointer to the entity structure being evaluated.
- `arg_4` (int, [ebp+0xC]) - Flag or mode selector influencing proximity and intersection test branches.
- `outPos` (Vector2*, [ebp+0x10]) - Pointer to a coordinate buffer where the resulting validated position is written upon success.

**Returns:** Returns true (1 in al) if all placement/action criteria and spatial checks pass, storing the resulting position; false (0 in al) otherwise.

**Side effects:**
- Writes validated 2D coordinates to the output buffer pointed to by arg_8 upon successful completion.

**Data structures:**
- Entity: +0x18 flags/state byte, +0x1C status pointer/flags, +0x34 position/transform data, +0x38 index/type configuration

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404F35  
  Called to perform a conditional check or complex operation on the entity, passing the entity pointer, an index from ebx+1, and a pointer to a local stack variable whose value is populated during the call.
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x0040537F  
  Called to check whether the entity (passed in edi) meets specific capability or prerequisite criteria identified by index esi, using an argument from the caller and a local work variable buffer (ptr at esp+var_8). Aborts if it returns false (al == 0).

**Calls:**
- `FindNearestValidEntityInRadius` (0x00402E50) - call at 0x004055B7
- `_CIsqrt` (0x0041A212) - call at 0x00405674, 0x004056CE, 0x00405955, 0x004059B5
- `Vec3_NormalizeOrZero` (0x00417FD0) - call at 0x0040577D
- `__ftol2_sse` (0x0041A160) - call at 0x0040579A, 0x004057D9
- `CheckGridAreaPassesCriteria` (0x00414CE0) - call at 0x004058C1
- `IntersectRaySegmentOrLine` (0x004182F0) - call at 0x00405920

### IntersectRaySegmentOrLine - `0x004182F0`

_original name `sub_4182F0` | `Vector2* __fastcall IntersectRaySegmentOrLine(void *this, void *edx_buffer)` | confidence: medium | heavy x87 floating-point math and conditional branching consistent with 2D geometry/intersection routines. | source: llm_

**Performs a 2D geometric intersection or closest-point/projection calculation (likely between a ray/line segment and a point or other geometric primitive), populating a coordinate buffer and returning its pointer.**

The function performs heavy floating-point arithmetic (x87 FPU) implementing a 2D geometric calculation such as a ray-segment intersection, line projection, or distance-based spatial query. It accesses input data via `ecx` and a buffer pointer passed in `edx`. Based on conditional checks (fcom / fnstsw / test ah, 5), it branches between alternative geometric handling paths (e.g., handling parallel lines, clamping parameters between 0 and 1 for segments, or computing intersection factors) before writing resulting coordinates into the `edx` buffer and returning `eax = edx`.

**Parameters:**
- `this_ptr` (void*, ecx) - Pointer to an object containing geometric parameters (source points, direction vectors, or shape data).
- `out_buffer` (Vector2*, edx) - Pointer to a local stack or heap buffer where resulting 2D coordinates or intersection points are written.

**Returns:** Returns the pointer to the output coordinate buffer in eax (equal to the input edx).

**Side effects:**
- Writes computed 2D coordinate values into the buffer pointed to by edx.

**Data structures:**
- Output Buffer: +0x0 float x, +0x4 float y

**Called by:**
- `CheckRaySegmentCollisionOrDistance` (0x004183C0) - call at 0x004183CA  
  Called once at the start of the function, passing a local stack buffer pointer in edx, to populate coordinate or state data used immediately afterward for floating-point distance and geometric calculations.
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x00405920  
  Called conditionally within a matching block (at 0x00405920) to compute relative vector or distance data between the entity and a candidate element using entity sub-structures and local coordinate variables.

### CheckEntityConditionsOrCount - `0x00404DF0`

_original name `sub_404DF0` | `int __stdcall CheckEntityConditionsOrCount(void *entity, int targetId)` | confidence: high | Disassembly is completely straightforward and self-contained with clear array strides and offset references. | source: llm_

**Queries entity state and loops through a list of sub-elements to calculate an accumulated count or conditional metric, combined with a timing/distance check based on a global data structure.**

The function takes an entity pointer and a target ID as arguments. It first reads a count from `entity + 0x244`. If this count is greater than zero, it iterates through an array or list of sub-elements located at `entity + 0x144`. For each element, it checks if `[esi + 0x70] == 0x0A` and whether a 16-bit field `[esi + 0x72]` matches the target ID. If it matches, it inspects another internal array (`esi + 0x210`), looks for elements satisfying a specific byte flag at offset `+0x10`, and validates that a pointer condition (`[eax + 0x1C] == 0`) holds. Successful matches increment an internal accumulator (`var_8`). After the loop, it performs an additional check by subtracting an array value at `entity + arg_4 * 4 + 0x48` from a global tick/time value stored in a global structure (`dword_42AC60 + 0x5BBC0`), comparing the difference against 1E (30 decimal). The final return value is the sum of the accumulated sub-element count and the result (0 or 1) of the time/threshold comparison.

**Parameters:**
- `entity` (void*, [esp+4] (arg_0)) - Pointer to the main entity object.
- `targetId` (int, [esp+8] (arg_4)) - Target ID or index used to filter sub-elements and access entity arrays.

**Returns:** An integer representing the combined count of matching sub-elements plus a binary flag indicating if a time/distance threshold has been met (returned in eax).

**Data structures:**
- Entity: +0x48 Array of int/values indexed by targetId, +0x144 Array/Pointer to sub-elements, +0x244 Count of sub-elements
- Sub-element: +0x5Eh byte field, +0x70h byte type/status, +0x72h word ID, +0x210h internal array/pointers
- Global struct at dword_42AC60: +0x5BBC0 time or counter value

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404ECF, 0x00404F0A  
  Called to query or retrieve a metric/count value associated with an entity and an index (passed in ebx+1 and edi/entity pointer), used to compute bounds for command validation.
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404D00  
  Called at 0x00404D00 when entity state counters and flags meet specific requirements, passing the main entity pointer in edi and a calculated index/parameter in esi on the stack.

### CheckAndExecuteUnitAbility - `0x00405110`

_original name `sub_405110` | `bool __stdcall CheckAndExecuteUnitAbility(void *entity, int abilityIndex)` | confidence: high | Function performs clear cooldown, prerequisite, target-selection, and dispatch steps based on well-defined table data. | source: llm_

**Validates resource cooldowns, unit requirements, and target constraints for a specific ability/action on an entity, and dispatches a command packet if all checks pass.**

The function first checks if a global time counter (stored at offset 0x5BBC0 from a global base pointer) has advanced sufficiently past the last-used timestamp for the given ability index (passed via `arg_4`, indexed into an array at `entity + 0x48`). If the cooldown has not expired, it returns false (0). Otherwise, it looks up a configuration structure from `.rdata` (base `0x41B178`) based on the ability index, verifying unit prerequisites, resource/cost limits (via `ValidateCapacityOrLimit`), and querying a collection of sub-objects/units in the entity's inventory/roster (from `entity + 0x3E0`) to find a valid target matching specific flags and state criteria. If a valid target is found, it dispatches an outgoing command packet using `DispatchCommandPacket` with command type 0x0B and updates the ability's last-used timestamp.

**Parameters:**
- `entity` (void *, [esp+4])
- `abilityIndex` (int, [esp+8])

**Returns:** Returns true (1) in al if the ability checks passed and the command was dispatched; otherwise false (0).

**Side effects:**
- Updates the cooldown timestamp array within the entity structure at `[entity + abilityIndex * 4 + 0x48]`.
- Sends a network command packet via `DispatchCommandPacket` if preconditions are met.

**Data structures:**
- Entity: +0x48 cooldown timestamp array, +0x3E0 array of sub-object pointers, +0x4E0 sub-object count, +0x4E0 capacity/limit fields

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404F97, 0x00404FC9  
  Called to execute a specific action or state transition on the entity, passing the entity pointer and an index parameter (from ebx+1).
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404CB5  
  Called at 0x00404CB5 under specific threshold and comparison checks involving entity attributes and configuration indices, passing the main entity pointer in edi and an index value as an argument.

**Calls:**
- `ValidateCapacityOrLimit` (0x00404260) - call at 0x004051A5
- `DispatchCommandPacket` (0x004046B0) - call at 0x0040520C

### ValidateCapacityOrLimit - `0x00404260`

_original name `sub_404260` | `bool __fastcall ValidateCapacityOrLimit(void *entity, int arg_0)` | confidence: high, clean and straightforward arithmetic and comparison logic with clear semantics. | source: llm_

**Validates whether a calculated value based on an entity's fields and an argument is within a capped limit (0x64), returning a boolean gating further execution.**

The function reads a 32-bit integer from offset +0x10 of the entity structure passed in ecx, comparing it against the constant 0x64 (100). If the value is greater than or equal to 0x64, it is capped at 0x64. It then retrieves another integer from offset +0xc of the entity, adds the function argument (passed via stack at [esp+4]), and compares this sum against the capped limit. It returns true (1) if the sum is less than or equal to the limit, and false (0) otherwise.

**Parameters:**
- `this` (void*, ecx) - Pointer to the entity or object state structure.
- `arg_0` (int, [esp+4]) - An integer value (often derived from a command, bytecode, or table entry) added to the entity's base value for comparison.

**Returns:** bool in al, true (1) if the sum does not exceed the clamped limit of 100, false (0) otherwise.

**Data structures:**
- Entity: +0x0C int base_value, +0x10 int limit_value

**Called by:**
- `Entity::ProcessStateTransitionOrAction` (0x00404EA0) - call at 0x00404F8C  
  Called with the entity pointer in ecx and a byte value retrieved from the sub-structure as an argument to perform a validation check, returning a boolean in al that gates further execution.
- `CheckAndExecuteUnitAbility` (0x00405110) - call at 0x004051A5  
  Called conditional on a non-zero byte value loaded from a static table entry ([esi+2E'], converted via movsx to a signed integer) to perform an auxiliary check or sub-validation on the entity or action; the boolean return value (in al) is tested, and if zero, the main function aborts and returns failure (0).

### FindEntityInRange - `0x00402A30`

_original name `sub_402A30` | `void* __stdcall FindEntityInRange(float x, float y)` | confidence: high | Clear loops, spatial distance calculations, grid checks, and pointer array iteration typical of RTS entity management. | source: llm_

**Iterates through a global list of entities, checks their active state, distance to a specified 2D coordinate, and grid/pathfinding constraints, returning the first matching entity pointer or NULL.**

The function begins by retrieving a global manager/state pointer from `dword_42AC60`. It checks an entity count stored at `[ebp + 573B4h]`. If greater than zero, it loops through an array of entity pointers located at `[ebp + 573B0h]`. For each entity, it verifies that bit 0 of `byte ptr [entity + 18h]` is NOT set (checking an active/valid flag). It then calculates the squared 2D Euclidean distance from the input coordinates (`arg_0`, `arg_4`) to the entity's position, comparing it against a threshold radius (the FPU comparison sequence computes distance squared and tests it). If within range, it performs grid coordinate checks via a loop using lookup tables `dword_41D31C` and `dword_41D32C` combined with entity properties (`word ptr [entity + 1c]` scaled by 0x60). If all spatial/grid condition tests pass, it selects that entity, halts the iteration, and returns a pointer to it in `eax`. Otherwise, it completes the loop and returns `NULL`.

**Parameters:**
- `x` (float, [esp+0] / FPU stack) - X coordinate to check distance against.
- `y` (float, [esp+4] / FPU stack) - Y coordinate to check distance against.

**Returns:** Pointer to the matched entity object (or NULL), returned in eax.

**Data structures:**
- Global Manager: +0x573B0 int entity_array_ptr, +0x573B4 int entity_count, +0xD124 grid/map data matrix
- Entity Object: +0x18 byte flags (bit 0 = inactive/dead), +0x1C word grid/position index

**Called by:**
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404C1F  
  Called at 0x00404C1F with a pointer to [edi+34h] to check or query a condition/state. If it returns a non-zero pointer, fields from the result are used in subsequent entity processing.

### DispatchCommandPacket - `0x004046B0`

_original name `sub_4046B0` | `void __fastcall DispatchCommandPacket(void *entity, void *edx_unused, void *sourceData, int arg0, char arg4, char arg8, int argC, short arg10)` | confidence: high | well-supported by callee signatures (BitStream_WriteBits, SerializeCommandPacket) and typical RTS network packet dispatch patterns | source: llm_

**Constructs and serializes an outgoing network command packet using a local bitstream buffer, checking global packet-rate limits before transmission.**

The function builds a local command packet structure on the stack using provided parameters (entity pointer, source data pointers, and multiple flags/arguments passed via registers and stack). It accesses a global state structure at `dword_42AC60` and checks if a rate-limiting or counter value at offset `+0x5472C` exceeds `3C00h` (15360). If the threshold is exceeded, it increments an error or drop counter at `+0x5473Ch` and aborts early. Otherwise, it writes bit flags using `BitStream_WriteBits`, serializes the command packet via `SerializeCommandPacket`, and increments a sent packet counter at `+0x54734h`.

**Parameters:**
- `entity` (void*, edx) - Primary entity or context pointer (byte at [edx] used to initialize packet byte)
- `sourceData` (void*, eax) - Pointer to source data/sub-object used to populate packet fields
- `arg_0` (int, esp+5Ch) - Integer argument or ID
- `arg_4` (char, esp+60h) - Byte parameter / flag
- `arg_8` (char, esp+64h) - Byte parameter / flag
- `arg_C` (int, esp+68h) - Integer parameter or identifier
- `arg_10` (short, esp+6Ch) - Short parameter or index

**Returns:** none

**Side effects:**
- Modifies global statistics counters at dword_42AC60 (+0x54734h and +0x5473Ch)
- Serializes and writes data into a bitstream buffer

**Called by:**
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x00407ADC  
  Invoked when a sub-component passes cooldown and validity checks to execute a primary action or activation event, passing the entity pointer in edx, target/sub-object pointers via stack arguments, and calculated integer parameters.
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x0040742E, 0x00407737, 0x00407976  
  Called to execute or trigger the ability/action/spell targeting the selected entity and parameters (passing entity pointers, target data, and action type 0Dh).
- `CUnit::EvaluateCombatActionOrAcquireTarget` (0x004070A0) - call at 0x00407108, 0x004071B3  
  Invoked to perform a primary unit action, command, or event dispatch (called with parameters including esi as the entity/source, ebp as an argument, constants like 0, 8, 0E_h, 0F_h, and pointers derived from components or slots).
- `CheckAndDispatchAction` (0x00405230) - call at 0x004052E2  
  Invoked as a final action/dispatch step when all validation checks and loop conditions over the entity's sub-components pass successfully, receiving the entity pointer in edx, a pointer offset in eax, and pushing an integer constant (0Ch) along with three zero arguments and the index esi before updating the entity state.
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404BFB, 0x00404C48  
  Called twice within the main entity loop (at 0x00404BFB and 0x00404C48) to perform an action or trigger a notification/dispatch on a sub-element, passing the entity pointer in edx, source data in eax/ebx, and several flags or computed parameters onto the stack.
- `Entity::TryPerformActionOrCommand` (0x00405300) - call at 0x004053A6  
  Called as the main execution step after all checks pass, passing the entity pointer (edi), the result from sub_4053D0, and several constant parameters (0Ah, 4, 0, esi) along with the local variable buffer reference to execute the action or transaction.
- `CheckAndExecuteUnitAbility` (0x00405110) - call at 0x0040520C  
  Called to execute or dispatch the validated action once a suitable sub-object or target has been successfully identified from an internal collection. It is passed the primary entity pointer (in edx), the selected sub-object pointer (in ecx), an offset/identifier from the sub-object (lea eax, [ecx+8]), an integer constant 0Bh, two zero arguments, and the action/index argument passed into the function (arg_4 via esi).

**Calls:**
- `BitStream_WriteBits` (0x00418AA0) - call at 0x0040472F
- `SerializeCommandPacket` (0x004087D0) - call at 0x0040473B

### SerializeCommandPacket - `0x004087D0`

_original name `sub_4087D0` | `bool __fastcall SerializeCommandPacket(BitStream *stream, const CommandPacket *packet)` | confidence: high | structural layout and sub-calls strongly indicate packet bit-stream serialization logic | source: llm_

**Serializes a complex command or action packet structure into a bitstream using bit-packing, varints, and normalized floats.**

The function serializes a structured command/action buffer (passed via ebp, with stream in esi) into a bitstream using a combination of fixed-width bit fields, variable-length integers (varints), bit flags, and normalized floating-point values. First, it writes header bytes and an array of delta-encoded integer values using BitStream_WriteBits and WriteBitStreamVarInt. It then checks error/overflow states on the bitstream and manually packs various flag bitfields (at offsets +0x34 and +0x35 of the packet struct) into the buffer with explicit bitwise shift and mask operations. Finally, it appends trailing varints and normalized floats (using BitStream_WriteNormalizedFloat) based on configuration flags, returning true if the bitstream buffer did not overflow during serialization.

**Parameters:**
- `stream` (BitStream *, esi) - Pointer to the target bitstream context/buffer.
- `packet` (const CommandPacket *, [esp+8]) - Pointer to the command packet structure to be serialized.

**Returns:** Returns true (1) if the bitstream buffer did not encounter an overflow, or false (0) otherwise; returned in al/eax.

**Side effects:**
- Writes serialized binary data into the BitStream buffer pointed to by esi
- Updates bitstream offset and overflow flag fields within the BitStream structure

**Data structures:**
- BitStream: +0x0 data pointer, +0x4 buffer size/capacity, +0x8 bit offset, +0x0C overflow/error flag
- CommandPacket: +0x0/+0x1/+0x2/+0x3 header/count bytes, +0x4 array of integer values, +0x34/+0x35 flag bytes, +0x36 word value, +0x38 extra parameter

**Called by:**
- `SerializeCommandBatch` (0x00404760) - call at 0x00404852  
  Called inside the main processing loop to submit or dispatch the constructed local packet/request structure (passed as a pointer on the stack via `lea eax, [esp+58h+var_48]`).
- `Entity_CheckAndSerializeCommand` (0x00406EF0) - call at 0x0040706E  
  Called after sub_418AA0, passing a pointer to a locally constructed parameter/command block on the stack (starting at var_48) initialized with entity attributes and state flags, to submit or process the action request.
- `DispatchCommandPacket` (0x004046B0) - call at 0x0040473B  
  Called with a pointer to the locally constructed parameter/packet buffer (at [esp+var_48]), receiving an object or status result in esi from the resource returned by sub_418AA0, as part of dispatching the action or event.

**Calls:**
- `BitStream_WriteBits` (0x00418AA0) - call at 0x004087E0, 0x004087EC, 0x004087F8, 0x0040880E
- `WriteBitStreamVarInt` (0x00418B10) - call at 0x0040883D, 0x004089A7, 0x004089B5
- `BitStream_WriteNormalizedFloat` (0x00418C80) - call at 0x004089D0, 0x004089E5

### BitStream_WriteNormalizedFloat - `0x00418C80`

_original name `sub_418C80` | `void __stdcall BitStream_WriteNormalizedFloat(BitStream *stream, float val, float min_val, float max_val, int num_bits)` | confidence: high | standard pattern for networked game engines encoding ranged floats into fixed bit-widths via bitstreams. | source: llm_

**Serializes and writes a floating-point value clamped to a specified range into a bitstream as a quantized fixed-point integer.**

The function takes a floating-point value along with range boundaries (min and max) and a bit-width count. It normalizes and scales the floating-point value to a quantized integer range, clamps it between 0.0 and 1.0 using x87 FPU comparison logic, multiplies it by the maximum integer representable by the given bit count, rounds it to the nearest integer using custom FPU control word manipulation, and finally invokes BitStream_WriteBits to write the resulting integer into the bitstream.

**Parameters:**
- `stream` (BitStream*, stack / register per calling convention) - The bitstream context pointer to write to.
- `val` (float, x87 stack / argument) - The floating-point value to serialize.
- `min_val` (float, x87 stack / argument) - The lower bound of the normalization range.
- `max_val` (float, x87 stack / argument) - The upper bound of the normalization range.
- `num_bits` (int, stack) - The number of bits used to quantize the value.

**Returns:** none

**Side effects:**
- Modifies the state of the provided BitStream buffer by writing quantized bits.

**Called by:**
- `SerializeCommandPacket` (0x004087D0) - call at 0x004089D0, 0x004089E5  
  Called conditionally based on bit flags in the parameter structure, passing the serialization context pointer (esi) alongside floating-point values to serialize extended attributes or coordinate data.

**Calls:**
- `BitStream_WriteBits` (0x00418AA0) - call at 0x00418CFA

### WriteBitStreamVarInt - `0x00418B10`

_original name `sub_418B10` | `void __fastcall WriteBitStreamVarInt(BitStreamContext *ctx, unsigned int value)` | confidence: high | The bitwise chunking logic (7 bits data + 1 continuation bit) combined with stream offset tracking and bounds checking precisely matches variable-length bitfield serialization routines. | source: llm_

**Serializes an integer value as a variable-length bit-packed integer (varint-style encoding using 7-bit chunks with continuation bits) into a bitstream buffer.**

The function takes a context pointer in `eax` and an integer value in `ecx`. It loops, extracting 7-bit chunks from the value and setting an 8th continuation/more bit if more chunks remain. The context structure holds a pointer to the byte buffer (`+0`), the maximum capacity in bits (`+4`), the current bit write offset (`+8`), and an error/overflow flag (`+0Ch`). For each 7-bit chunk, it checks if sufficient space remains in the buffer, sets the error flag if an overflow occurs, and otherwise writes the bits into the byte-level buffer starting at the current bit index using a sequence of repeated bit-setting blocks for up to 8 bits per chunk. It repeats this process in a loop until the entire value has been shifted out and written.

**Parameters:**
- `ctx` (BitStreamContext *, eax) - Pointer to the serialization context/buffer state structure.
- `value` (unsigned int, ecx) - The integer value to be packed and written to the bitstream.

**Returns:** none

**Side effects:**
- Modifies the bitstream buffer memory pointed to by ctx->buffer
- Updates the bit write offset at ctx->+8
- May set the error/overflow flag at ctx->+0Ch if buffer bounds are exceeded

**Data structures:**
- BitStreamContext: +0x0 void* buffer, +0x4 unsigned int max_bits, +0x8 unsigned int current_bit_offset, +0xC char overflow_flag

**Called by:**
- `SerializeCommandPacket` (0x004087D0) - call at 0x0040883D, 0x004089A7, 0x004089B5  
  Called to write integer or bit-length fields (such as calculated bounds or word values from the parameter structure) into the serialization stream, receiving the context pointer in eax/esi and value in ecx.

### BitStream_WriteBits - `0x00418AA0`

_original name `sub_418AA0` | `void __fastcall BitStream_WriteBits(BitStream *stream, int value, int count)` | confidence: high, clear bit manipulation and buffer serialization pattern matching a bitstream context | source: llm_

**Writes a specified number of bits from an integer value into a bitstream buffer, updating bit offsets and tracking overflow status.**

The function first checks an overflow flag at offset +0x0C of the stream object. If the flag is already set, it returns immediately. Otherwise, it calculates the new bit position by adding the requested bit count to the current bit count at offset +0x8, comparing it against the maximum capacity (derived from offset +0x4 multiplied by 8). If the write would exceed capacity, the overflow flag is set and the function returns. If within bounds, it loops 'count' times, testing each bit of the input value and setting the corresponding bit in the byte array buffer (located via base pointer at offset +0x0) by computing byte and bit offsets, advancing the bit stream pointer accordingly.

**Parameters:**
- `stream` (BitStream*, eax) - Pointer to the bitstream context structure.
- `value` (int, [esp+4]) - The integer value containing the bits to be written.
- `count` (int, [esp+8]) - The number of bits to write from the value.

**Returns:** none

**Side effects:**
- Modifies the bitstream buffer memory at the current bit position
- Updates bit count and overflow flag fields within the BitStream structure

**Data structures:**
- BitStream: +0x0 byte* buffer, +0x4 int capacity_bytes, +0x8 int current_bit_offset, +0x0C byte overflow_flag

**Called by:**
- `ResourceBuffer_FlushOrWritePacket` (0x00408B60) - call at 0x00408B6F  
  Called unconditionally at the start of the function with two arguments (0 pushed, and `edi + 2824h` passed in eax via ebp) to perform an initialization or setup operation on a sub-structure or stream associated with the resource block.
- `Entity_CheckAndSerializeCommand` (0x00406EF0) - call at 0x00407062  
  Called with two integer/flag arguments (both pushed as 1) and a base pointer derived from global data (edi+54724h) to allocate, retrieve, or initialize a global resource or buffer manager object when the system time/counter threshold is met.
- `DispatchCommandPacket` (0x004046B0) - call at 0x0040472F  
  Called with two integer constants (1, 1) and a pointer derived from a global structure ([edi+54724h]) to allocate or retrieve a resource/buffer/object prior to packet processing.
- `SerializeCommandPacket` (0x004087D0) - call at 0x004087E0, 0x004087EC, 0x004087F8, 0x0040880E  
  Called multiple times to write initial header fields or primitive values from the parameter structure (bytes from ebp) into the serialization stream, passing values and size/type specifiers via stack arguments with the context pointer in esi.
- `BitStream_WriteNormalizedFloat` (0x00418C80) - call at 0x00418CFA  
  Called after clamping and rounding a processed floating-point value to an integer, passing the integer result (pushed via eax) along with context parameters to finalize writing or packing that value into the bitstream buffer.

### FindNearestValidEntityInRadius - `0x00402E50`

_original name `sub_402E50` | `void* __stdcall FindNearestValidEntityInRadius(float x, float y)` | confidence: medium, based on standard spatial proximity search patterns over entity lists with health and flag checks | source: llm_

**Iterates through a global list of entities, filtering by flags, health, and a squared distance radius, and returns a pointer to the closest matching entity (or null if none found).**

The function retrieves a global manager structure from `dword_42AC60` which contains an array of entity pointers and a count. It accepts coordinates (or distance criteria) passed via stack/FPU arguments. It unrolls the entity processing loop in chunks of 4 (using a vectorized-like pattern over blocks of 4 pointers), followed by a cleanup loop for any remaining items. For each entity, it checks several criteria: whether a flag byte at offset +1Fh (or via pointer +1Ch) has bit 7 set, whether flag bits at +18h match specific mask criteria, and whether the integer at +28Ch (or +2B8h) is greater than zero (representing health or active state). If these checks pass, it computes the squared Euclidean distance from the target coordinates, compares it against a threshold or current minimum, and tracks the entity that meets the criteria with the closest distance.

**Parameters:**
- `y` (float, stack arg_0 (via ebp)) - Target Y coordinate or search parameter
- `x_or_radius` (float, FPU stack (st(0)) on entry) - Target X coordinate or radius threshold

**Returns:** Pointer to the nearest matching entity object, or NULL (0) in eax

**Data structures:**
- Global Manager: +0x5738C entity array pointer, +0x57390 entity count
- Entity Object: +0x18 flags/state, +0x1C status flags pointer/byte, +0x2B8 health/status counter

**Called by:**
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404B3A, 0x00404CC8  
  Called twice (at 0x00404B3A and 0x00404CC8) to query or retrieve an associated object/target related to coordinates or state offset [edi+34h]. The returned pointer is checked for validity (non-zero) to determine subsequent conditional execution paths or status thresholds.
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x004055B7  
  Called once near the beginning of the function (at 0x004055B7) to initialize or retrieve state data using a local parameter block containing pointers and configuration values derived from the entity and global tables.

### UpdateEntitySelectionAndLists - `0x00404940`

_original name `sub_404940` | `void __cdecl UpdateEntitySelectionAndLists(Entity *entityStruct)` | confidence: high, clear iteration pattern over global entity list with structured classification flags and array insertions. | source: llm_

**Iterates through a global list of game units/entities, filtering and categorizing them into various sub-lists and tracking counts within the specified entity structure.**

The function initializes several counters and list sizes within the passed entity structure (in eax) to zero. It then retrieves a global entity manager or collection pointer from `[0x0042AC60]` which contains a count (`+0x57390`) and an array of entity pointers (`+0x5738C`). It loops through each global entity, checking if its type byte (`[ecx+0x1B]`) matches the target identifier stored at `[eax]`. For matching entities, it inspects various flags, states, and subtypes (such as flags at offsets `+0x18`, `+0x1C`, and sub-struct values) to classify them. Depending on these flags, entities are added to distinct internal arrays/lists (such as selection/command queues at offsets `+0x144`, `+0x248`, `+0x3D0`, `+0x3E0`, and `+0x4E4`) up to specific capacity limits. Finally, if any entities were added to a specific category, it updates primary target or coordinate fields (`+0x34`, `+0x38`) using data from the first matched entity.

**Parameters:**
- `entityStruct` (Entity*, eax) - Pointer to the main entity/selection structure being updated.

**Returns:** none

**Side effects:**
- Modifies fields and collection arrays within the Entity structure passed in eax

**Data structures:**
- Entity: +0x0 target type byte, +0x34 target/position X, +0x38 target/position Y, +0x144... entity pointer lists and counters

**Called by:**
- `CManager::UpdateEntitiesOrUnits` (0x00403460) - call at 0x0040349F  
  Called inside the loop for the current entity (with `eax` pointing to `edi`, the entity structure at `esi+630h + ebp*4FCh`) when interval and flag conditions are met.

### CheckSimulationBoundsOrState - `0x004033A0`

_original name `sub_4033A0` | `void __fastcall CheckSimulationBoundsOrState(void *resourcePtr)` | confidence: high, based on clear x87 floating-point instruction blocks, branch analysis, and array-loop structure over known offsets in edx. | source: llm_

**Evaluates floating-point conditions and bounds, and conditionally sets flags on an array of sub-objects stored within a large resource structure.**

The function performs a series of x87 floating-point comparisons and arithmetic operations against constants located in the .rdata section. It evaluates comparative states (similar to checking ranges or wrapping conditions) and sets a boolean condition in cl based on CPU status flags derived from fnstsw/test instructions. It then compares another set of floating-point values to determine an integer result in eax. If the boolean condition in cl does not match eax, and an integer count at offset +0x57390 of the resource structure provided in edx is greater than zero, it enters a loop. This loop iterates from 0 up to that count, fetches pointers from an array at offset +0x5738Ch, and sets a specific flag bit (0x400) at offset +0x18 of each targeted sub-object.

**Parameters:**
- `resourcePtr` (void *, edx) - Base pointer to the resource structure containing state data and a sub-object array.

**Returns:** none

**Side effects:**
- Modifies flags (sets bit 0x400 at offset +0x18) on objects within an array inside the resource structure if specific floating-point conditions are met.

**Data structures:**
- Resource structure: +0x5738C pointer to an array of sub-object pointers, +0x57390 integer count of sub-objects.
- Sub-object structure: +0x18 flags or status word where bit 0x400 is set.

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D87  
  Called unconditionally as part of the update sequence, receiving the base resource pointer in edx (ebp).

### ProcessPriorityQueueTick - `0x00419300`

_original name `sub_419300` | `void __fastcall ProcessPriorityQueueTick(int *pqStructure)` | confidence: high | complete disassembly mapping with clear interactions with known heap, allocation, and sift-down subroutines | source: llm_

**Extracts elements from a binary heap / priority queue, invokes a callback function on them under certain conditions, and re-inserts or manages elements in a loop until the queue is empty or iteration limits are reached.**

The function implements a processing loop over a priority queue (or event queue) structure passed via `ebx`. In each iteration, it checks if the queue has elements and extracts the root/top element (storing its 24-byte payload into local variables). It then adjusts the heap using `HeapSiftDown` [sub_419690]. Depending on whether a callback pointer is present at offset +0x8 in the queue structure, it invokes the callback passing extracted data. Based on the callback's return values (handled via FPU comparisons), it may dynamically resize the underlying buffer using `AllocateCustomBlock` [sub_4185D0] or `ReallocateCustomBlock` [sub_418670], copy elements using `memcpy`, free memory via `free`, and push new or updated elements back onto the heap using `HeapUp` [sub_419590]. The loop continues up to a maximum of 256 iterations (0x100) or while the queue is non-empty.

**Parameters:**
- `pqStructure` (int*, ebx) - Pointer to the priority queue or event structure (+0x0: buffer, +0x4: current item count, +0x8: capacity/callback).

**Returns:** none

**Side effects:**
- Modifies the priority queue structure in ebx (item counts, buffer pointers, capacity)
- Allocates, reallocates, and frees memory blocks via custom block allocators and MSVCR80!free
- Invokes user-defined callbacks stored in the queue structure

**Data structures:**
- PriorityQueue: +0x0 void* buffer, +0x4 int count, +0x8 callback_or_capacity
- QueueElement: 24 bytes (0x18) per entry, containing function pointers, data arguments, and state fields.

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D80  
  Called unconditionally as part of the core per-tick update routine, operating on state near [ebp+51E90h] with floating-point values prepared on the FPU stack.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004193E8, 0x0041941E
- `memcpy` (0x0041A22A) - call at 0x0041943B
- `HeapSiftDown` (0x00419690) - call at 0x00419498
- `ReallocateCustomBlock` (0x00418670) - call at 0x00419526
- `sub_419590` (0x00419590) - call at 0x0041956F

### HeapSiftDown - `0x00419690`

_original name `sub_419690` | `void __cdecl HeapSiftDown(void *heapStructure)` | confidence: high | Binary heap sift-down pattern, index calculations ($2i+1$, $2i+2$), and in-place element swapping of fixed-size records are textbook characteristics of a heap adjustment routine. | source: llm_

**Performs a binary heap sift-down (or heapify-down) operation on a collection passed in edi, restoring the heap property by moving elements down.**

The function implements standard binary heap sift-down logic (often associated with priority queues or binary heaps). It takes a heap structure container passed via the edi register (where `[edi]` points to the underlying array of elements and `[edi+4]` represents the current count/size of the heap). It iteratively compares a parent node with its left and right children, determines the appropriate child based on comparison metrics (utilizing floating-point comparisons and secondary integer ID/priority ties at offset +0x14), swaps the parent with the selected child element (each element being 24 bytes, or 0x18 bytes wide, indexed via `lea` with scaling), and continues downward until the heap property is satisfied.

**Parameters:**
- `heapStructure` (struct Heap *, edi) - Pointer to the heap container object, containing the data array pointer at offset +0 and size at offset +4.

**Returns:** none

**Side effects:**
- Modifies the element array within the heap structure in-place by reordering elements to restore the heap property.

**Data structures:**
- Heap: +0x0 void* dataArray, +0x4 int count
- HeapElement: 24 bytes (0x18) in size, with key/value data and a secondary tie-breaker or priority field at offset +0x14.

**Called by:**
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x00419498  
  Performs adjustment, heap-down, or reordering operations on the elements in the collection after an item is removed or modified, passing the base structure in edi.

### RtsManager::ProcessNetworkStreamBatches - `0x00408C10`

_original name `sub_408C10` | `__stdcall int RtsManager::ProcessNetworkStreamBatches(void *rtsManager, int networkStreamId)` | confidence: high based on clear control flow, callee signatures, and caller context. | source: llm_

**Iterates through serialized packet or command batches from a network stream buffer, deserializes object states, and dispatches command batches to the RTS manager.**

The function begins by computing an index or table offset using the provided `networkStreamId` (passed in `ecx`), indexing into a resource/buffer structure passed as `arg_0` (`ebp`). It sets up a local iteration structure containing a bitstream/pointer reference and length field. Inside a loop, it checks bit flags within a bitfield buffer to determine if elements are active or valid. For each valid entry, it calls `DeserializeObjectState` (`sub_408A00`) to unpack state data onto the stack, and if successful, invokes `RtsManager::ProcessCommandOrActionBatch` (`sub_408D70`) to handle the commands. If deserialization fails, an error counter at offset `+0x283C` of the manager structure is incremented. The loop continues until the bit/index count exceeds the total length or a flag condition is met.

**Parameters:**
- `rtsManager` (void *, arg_0 ([esp+60h+arg_0])) - Pointer to the main RTS manager or simulation state structure.
- `networkStreamId` (int, ecx) - Stream identifier or index used to look up sub-buffers and state.

**Returns:** int (eax), returning completion status or final iteration count.

**Side effects:**
- Deserializes network data from stream buffers
- Dispatches game commands and actions via RtsManager::ProcessCommandOrActionBatch
- Increments error counters in the RTS manager structure on failure

**Data structures:**
- RtsManager: +0x283C error counter / failed packet count

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403D6B  
  Called when a certain step-based check passes (counter % 5 == 0, with edx == 0), passing a pointer to a sub-buffer at [ebp+51F00h] as an argument and using a value from [ebp+5BBC4h].

**Calls:**
- `DeserializeObjectState` (0x00408A00) - call at 0x00408CA4
- `RtsManager::ProcessCommandOrActionBatch` (0x00408D70) - call at 0x00408CB1

### RtsManager::ProcessCommandOrActionBatch - `0x00408D70`

_original name `sub_408D70` | `void __stdcall RtsManager::ProcessCommandOrActionBatch(void *rtsManager, void *commandPacket)` | confidence: high | complete CFG reconstruction and clear alignment with caller context and callee prototypes | source: llm_

**Iterates over a batch of entity commands or resource requests packed within a structure, validates them against game state and entity references, and dispatches them to appropriate handlers (such as component upgrades, resource parameter checks, or entity interaction processors).**

The function begins by inspecting header fields in the command packet (`ebp`). It checks type bounds and global feature flags, early-exiting and incrementing a failure counter if validation fails. It then loops over a count of sub-items located within the packet. For each item, it resolves an entity or resource handle by indexing into global entity lookup tables using encoded handles (splitting index and validation salt). Once a valid target entity/resource is confirmed and state flags (`+0x1A`, `+0x1B`, `+0x18`) are verified, it dispatches based on the command type byte (`[ebp+0]`): case 0 processes general entity interactions/commands via `ProcessEntityInteractionOrCommand` (optionally calling `CalculateGridCoordinatesOrMapping` if coordinate mapping is needed); case 1 performs component upgrades via `RtsManager::UpgradeComponent`; case 2 finds and updates resource entries via `FindResourceEntryByParam`. Successful processing increments a success counter at `[rtsManager+0x2838]`, whereas failures increment `[rtsManager+0x283C]`. Cleanups are handled via `retn 8` (__stdcall convention taking two dword arguments).

**Parameters:**
- `rtsManager` (void*, [esp+2Ch+arg_0]) - Pointer to the RTS manager object containing statistics counters at +0x2838 and +0x283C.
- `commandPacket` (void*, [esp+2Ch+arg_4]) - Pointer to a packet structure containing command type, sub-item count, entity/resource handles, and parameters.

**Returns:** none

**Side effects:**
- Increments success and failure counters on the RTS manager object at offsets +0x2838 and +0x283C.
- Triggers entity interactions, component upgrades, or resource parameter updates.

**Data structures:**
- CommandPacket: +0x0 byte commandType, +0x1 byte subType, +0x2 byte itemCount, +0x4 dword[] itemHandlesOrData, +0x34 dword parameters...

**Called by:**
- `RtsManager::ProcessNetworkStreamBatches` (0x00408C10) - call at 0x00408CB1  
  Called inside the element-processing loop when the preceding check on sub_408A00 succeeds. It takes the base resource pointer (ebp) and a pointer to a local buffer/state variable (esi / &var_48), likely operating on or updating the current active element.

**Calls:**
- `FindResourceEntryByParam` (0x0040BED0) - call at 0x00408E54
- `RtsManager::UpgradeComponent` (0x0040F3D0) - call at 0x00408E9C
- `CalculateGridCoordinatesOrMapping` (0x00408CE0) - call at 0x00408EEC
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x00408F15

### CalculateGridCoordinatesOrMapping - `0x00408CE0`

_original name `sub_408CE0` | `void __cdecl sub_408CE0()` | confidence: medium, determined to be a mathematical scaling/mapping helper using x87 FPU and integer root approximation, though exact semantic types of parameters passed via registers/stack are partially implicit. | source: llm_

**Calculates coordinate mapping or grid indices using an integer square root approximation loop followed by floating-point arithmetic operations.**

The function first determines an integer value 'ecx' such that ecx * ecx is less than or equal to the input value 'esi' (acting as an integer square root / grid dimension finder). It then performs various signed division and modulo operations, loads floating-point constants from the .rdata section, and executes a complex sequence of x87 FPU stack instructions (fild, fmul, fsubrp, fadd, etc.) to compute scaled floating-point coordinates or interpolation weights.

**Returns:** st(0) (implicit float/double return via x87 register)

**Called by:**
- `RtsManager::ProcessCommandOrActionBatch` (0x00408D70) - call at 0x00408EEC  
  Called during operation type 0 when secondary condition checks pass on command parameters, passing a pointer to local stack parameters (`ebp+3Ch` in ebx and `var_1C` in edi) to transform, format, or initialize command arguments before final dispatch.

### RtsManager::UpgradeComponent - `0x0040F3D0`

_original name `sub_40F3D0` | `bool __fastcall RtsManager::UpgradeComponent(void *this_ebx, int edx_unused, int componentIndex_eax)` | confidence: high | complete disassembly available and high-level RTS architecture clearly maps out structure field meanings | source: llm_

**Validates and advances a specific component level within an RTS manager object, optionally instantiating or updating the underlying component instance via the component factory.**

The function first validates the provided component index (passed in `eax`, must be <= 2) and checks resource/availability limits on the manager structure (`ebx`). It checks state flags and ensures a level progression limit is not exceeded. If valid, it increments the component's level counter at `[ebx + edi + 0x260]` and decrements a resource counter at `[ebx + 0x258]`. It then searches through an array of existing component pointers at `[ebx + 0x210]` to find one matching a type identifier stored in a lookup structure. If an existing component is found, its level property at offset `0x11` is updated. If not found and the count of active components is below 5, it calls `RtsComponentFactory_Create` to instantiate a new component object, stores it in the array, updates the active count at `[ebx + 0x5E]`, and invokes a virtual method on the newly created object. Finally, it sets a status bit (`0x400`) in the manager's flags word at `[ebx + 0x18]` and returns `true` on success, or `false` if validation fails.

**Parameters:**
- `this` (void*, ecx) - Pointer to the RTS manager object
- `componentIndex` (int, eax) - The index/type of the component being upgraded (0 to 2)

**Returns:** bool (1 in al on success, 0 on failure)

**Side effects:**
- Modifies component levels, resource counts, and component pointers within the manager structure
- Allocates new sub-components using RtsComponentFactory_Create when upgrading a component type for the first time
- Updates manager status flags at [ebx + 0x18]

**Data structures:**
- RtsManager: +0x18 word flags, +0x5E byte activeComponentCount, +0x210 array of component pointers, +0x24C pointer to data/config table, +0x258 resource counter, +0x260 component level array

**Called by:**
- `RtsManager::ProcessCommandOrActionBatch` (0x00408D70) - call at 0x00408E9C  
  Called inside the main loop for operation type 1, passing `edi` (word from [ebp+36h]) after verifying a flag on an internal sub-structure, to perform the requested command-specific action and increment a success counter.

**Calls:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0040F465

### DeserializeObjectState - `0x00408A00`

_original name `sub_408A00` | `bool __cdecl DeserializeObjectState(BitStream *stream, ObjectState *stateStruct)` | confidence: high | complete control flow and callee usage mapped, recognizing standard network/snapshot serialization patterns including zig-zag decoding and bitfield checks | source: llm_

**Deserializes serialized object network or snapshot state from a bitstream into a structure, validating length and header constraints.**

The function reads a series of bit fields and variable-length encoded values from an input bitstream context (passed via eax/ebx) into a destination structure pointed to by esi. It first reads four initial bit fields, validating that the first byte-sized field is <= 12 and the second is < 3. If valid, it loops a number of times (specified by the second byte read) to decode variable-length values using zig-zag decoding (`BitStream_ReadVariableLengthValue`). Next, it reads additional bit fields, an optional variable-length value (if a bitmask condition matches), and optionally parses floating-point scaling or position/rotation components using constants from .rdata/.data if specified by a flag. Finally, it checks a byte field at `[ebx+0Ch]` to determine the success return value (returning true if zero, false otherwise), or early-exits returning false if the initial bounds checks fail.

**Parameters:**
- `stream` (BitStream *, eax (saved to ebx at function entry)) - Bitstream reader context.
- `stateStruct` (ObjectState *, esi) - Pointer to the destination structure where the decoded state fields are written.

**Returns:** bool in al (1 if successful and stream/state constraints pass, 0 otherwise)

**Side effects:**
- Advances the read position of the provided bitstream context
- Populates the fields of the object state structure passed via esi

**Data structures:**
- ObjectState: +0x00 to +0x03 various parsed bit fields/bytes, +0x04.. array/vector values decoded via variable length, +0x34 byte, +0x35 byte, +0x36 WORD, +0x38.. additional integer and floating point state fields

**Called by:**
- `RtsManager::ProcessNetworkStreamBatches` (0x00408C10) - call at 0x00408CA4  
  Called inside a loop over elements of a bitmask-checked collection to retrieve or inspect data into a local buffer (passed via an address to a local structure, &var_58). The byte return value in al is tested to determine whether to proceed with processing the current item.

**Calls:**
- `StreamReadBits` (0x00418D10) - call at 0x00408A0C, 0x00408A1C, 0x00408A29, 0x00408A3A, 0x00408A8F, 0x00408A9C, 0x00408AF5, 0x00408B1D
- `BitStream_ReadVariableLengthValue` (0x00418D80) - call at 0x00408A5F, 0x00408AA8, 0x00408AD7

### BitStream_ReadVariableLengthValue - `0x00418D80`

_original name `sub_418D80` | `unsigned int __fastcall BitStream_ReadVariableLengthValue(BitStream *stream)` | confidence: high | The bit-manipulation patterns (shr/and/shl by 3 and 7 for byte/bit indexing) and loop structure clearly implement a variable-length bit-stream reader. | source: llm_

**Reads a variable-length bit-packed integer value from a bit-stream state structure passed in ebx.**

The function acts as a bit-stream reader, extracting variable-length encoded integer values from a buffer pointed to by the stream structure in ebx. It manages a bit-cursor (`ebx+8`) and a bit-limit (`ebx+4`), reading bits sequentially across byte boundaries using bitwise shifts and masks. It contains a loop (running twice when `var_10` is 2) that extracts 4-bit nibbles or chunks, combining them into an accumulated value. If the most significant bit of a chunk is set, it loops to read additional bytes, shifting the accumulated result until the continuation bit clears or an overflow condition is reached (setting an error flag at `ebx+0Ch`).

**Parameters:**
- `stream` (BitStream*, ebx) - Pointer to a bit stream state structure containing buffer pointer, bit count/limit, current bit offset, and error status.

**Returns:** The decoded integer value in eax.

**Side effects:**
- Advances the bit offset in the bit stream structure ([ebx+8]).
- May set an error/overflow flag at [ebx+0Ch] if the read exceeds the bit limit.

**Data structures:**
- BitStream: +0x0 void* buffer, +0x4 int bit_limit, +0x8 int bit_offset, +0xCh char error_flag

**Called by:**
- `DeserializeObjectState` (0x00408A00) - call at 0x00408A5F, 0x00408AA8, 0x00408AD7  
  Called at specific points during parsing (including within a loop bounded by a header byte) to read variable-length data or additional numerical values from the input stream, with results stored into fields of the structure at [esi].

### StreamReadBits - `0x00418D10`

_original name `sub_418D10` | `unsigned int __cdecl StreamReadBits(StreamContext *stream, int numBits)` | confidence: high | The bit-manipulation logic (div/mod 8 via shr 3 and and 7) combined with buffer bounds checks clearly implements a bitstream reader. | source: llm_

**Reads a variable number of bits (up to 32) from a bitstream context, advancing the read position and handling end-of-stream bounds.**

The function reads `edi` bits (passed in `edi`, representing `numBits`) from a custom stream structure passed in `ebx`. It first checks if the current bit offset (`[ebx+8] + edi`) exceeds the total available capacity (`[ebx+4] * 8`). If out of bounds, it clamps the bit pointer to the end, sets an error/EOF flag at `[ebx+0Ch]`, and returns 0. Otherwise, it loops `edi` times, extracting individual bits from a byte buffer located at `[ebx]`. Each bit is shifted into its corresponding position in a local accumulator variable (`var_4`), after which the stream's bit position pointer (`[ebx+8]`) is advanced by `edi` bits, and the accumulated integer value is returned.

**Parameters:**
- `stream` (StreamContext*, ebx) - Pointer to the stream context structure containing buffer pointer, size, and current bit position.
- `numBits` (int, edi) - Number of bits to read from the stream (typically 1 to 32).

**Returns:** unsigned int returned in eax, containing the bits read from the stream (or 0 on EOF/overflow).

**Side effects:**
- Modifies the stream read position at [ebx+8]
- May set an error or EOF flag at byte [ebx+0Ch]

**Data structures:**
- StreamContext: +0x0 void* buffer, +0x4 int sizeInBytes, +0x8 int currentBitOffset, +0x0C char errorOrEofFlag

**Called by:**
- `DeserializeObjectState` (0x00408A00) - call at 0x00408A0C, 0x00408A1C, 0x00408A29, 0x00408A3A, 0x00408A8F, 0x00408A9C, 0x00408AF5, 0x00408B1D  
  Called multiple times throughout the function (with various values loaded into edi beforehand, presumably as a stream reader/field identifier or context) to fetch bytes, integers, or status flags from the input stream into local variables and fields of the structure at [esi].

### InitializeSimulationAndResourceSubsystem - `0x00401750`

_original name `sub_401750` | `bool __stdcall InitializeSimulationAndResourceSubsystem(void *resourceBlock, unsigned int stateFlags)` | confidence: high | complete disassembly and clear subsystem initialization logic | source: llm_

**Initializes the primary resource block, game state subsystems, spatial grids, PRNG states, and entity registration tables before running initial simulation steps and event scripts.**

The function begins by taking the resource block pointer (ebx) and bounding the second argument (stateFlags) between 2 and 4, storing it at `[ebx+5BBBCh]`. It initializes the PRNG state via sub_418760, sets up a sub-manager via sub_4186E0, and zeros out various configuration and telemetry regions using memset. It then calls sub_401F60 to initialize subsystem records and performs an initial simulation/state update step using sub_416640. A loop then iterates through configured entity/resource definitions (up to `[ebx+100DCh]`), allocating custom memory blocks or managing pool chunks via sub_418470 / sub_4185D0 / malloc / free as needed, and initializes resource entries using sub_410A90. Following this, it spawns entity batches via sub_402050, iterates through spatial grid criteria via sub_414CE0 to create and register resource entities (sub_402560), processes path nodes/spawning via sub_407B50, registers event callbacks (sub_402980), and initializes entity registrations (sub_404570). Finally, it initializes state blocks via sub_4097E0, executes event scripts through the VM (sub_40A0D0), updates simulation grid regions (sub_4036B0), and returns true upon successful completion.

**Parameters:**
- `resourceBlock` (void*, [ebp+8]) - Pointer to the large main resource/simulation state block (~0x5BBE4 bytes).
- `stateFlags` (unsigned int, [ebp+0xC]) - Initialization flags/parameters bounded between 2 and 4.

**Returns:** bool (1 in AL on success, or 0 if state block initialization fails), returned in AL.

**Side effects:**
- Modifies fields across the entire 0x5BBE4 byte resource block
- Allocates memory pools and custom blocks via malloc and custom allocators
- Registers entity callbacks and executes initial virtual machine scripts

**Data structures:**
- ResourceBlock: +0x1A20 simulation grid/data, +0x51E80 sub-manager, +0x51EF8 PRNG state, +0x5BBD0 status flags

**Called by:**
- `main` (0x00401050) - call at 0x00401225  
  Called after memory allocation and initialization, passing the processed resource pointer and computed arguments (derived from command-line hash state); its boolean return value dictates whether another function (sub_403E50) is called.

**Calls:**
- `InitializePrngState` (0x00418760) - call at 0x00401786
- `InitializeSubManager` (0x004186E0) - call at 0x00401791
- `memset` (0x0041A150) - call at 0x004017E0, 0x00401865, 0x00401895, 0x0040192C
- `InitializeSubsystemRecords` (0x00401F60) - call at 0x004017ED
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x00401838
- `AllocateMemoryPoolBlock` (0x00418470) - call at 0x00401A28
- `ResourceEntry_Initialize` (0x00410A90) - call at 0x00401A86
- `AllocateCustomBlock` (0x004185D0) - call at 0x00401B42, 0x00401B78, 0x00401E31, 0x00401E67
- `memcpy` (0x0041A22A) - call at 0x00401B99, 0x00401E88
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x00401C42
- `__ftol2_sse` (0x0041A160) - call at 0x00401CA3, 0x00401CF2
- `CheckGridAreaPassesCriteria` (0x00414CE0) - call at 0x00401D31
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00401D64
- `ProcessRtsResourceSpawningOrPathNodes` (0x00407B50) - call at 0x00401DA9
- `EntityInteractionEventCallback` (0x00402980) - address taken at 0x00401DC0, 0x00401EE9
- `InitializeEntityRegistration` (0x00404570) - call at 0x00401F12
- `InitializeStateBlock` (0x004097E0) - call at 0x00401F2E
- `VM_ExecuteEntityEventScript` (0x0040A0D0) - call at 0x00401F44
- `UpdateSimulationGridRegions` (0x004036B0) - call at 0x00401F4C

### UpdateSimulationGridRegions - `0x004036B0`

_original name `sub_4036B0` | `void __fastcall UpdateSimulationGridRegions(void *resourceBlock)` | confidence: high based on clear loop structures, field offsets, and interaction with ProcessGridRegionAction | source: llm_

**Iterates through sub-components and grid regions in the simulation resource structure, clearing flags and conditionally invoking grid region updates.**

The function operates on a large resource/state block passed via the `esi` register. It first checks a count at `[esi+5BBBCh]`. If greater than zero, it loops through that many entries, advancing an internal base pointer (`ebp`) by 0x2400 bytes each iteration and clearing bit 0 (clearing the lowest bit via `and byte ptr [eax+ebp], 0FEh`) across a 2400-byte block. It then iterates through an item list of size `[esi+57390h]` stored at `[esi+5738Ch]`. For each item, it performs bitwise checks on flags at offset +0x18, verifies an index at offset +0x1B is within bounds (<= 5), and checks a bitmask at `[esi+0E8h]` using a shift derived from that index. If conditions pass, it extracts parameters from the item's sub-structure and invokes `sub_417B50 [ProcessGridRegionAction]` to update the grid region.

**Parameters:**
- `resourceBlock` (void *, esi) - Pointer to the main resource block containing grid data, item lists, and configuration flags.

**Returns:** none

**Side effects:**
- Modifies memory blocks within the resource structure including grid region buffers (+0x44680 onwards).
- Calls ProcessGridRegionAction for valid simulation items.

**Data structures:**
- Resource structure: +0x0E8 bitmask, +0x5738C item array pointer, +0x57390 item count, +0x5BBBC loop/count limit, +0x44680 grid region data block.
- Item structure: +0x18 flags, +0x1B type/index byte, +0x1C nested data pointer.

**Called by:**
- `RtsManager::UpdateSimulationTick` (0x00403D30) - call at 0x00403DC8  
  Called conditionally when the main counter [ebp+5BBC0h] is a multiple of 10 (counter % 10 == 0), receiving the resource pointer in esi (ebp).
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401F4C  
  Called near the end of the function with the resource block pointer (`arg_0`) as a final cleanup or finalization step before returning success (`1`).

**Calls:**
- `ProcessGridRegionAction` (0x00417B50) - call at 0x0040374C

### ProcessGridRegionAction - `0x00417B50`

_original name `sub_417B50` | `void __cdecl ProcessGridRegionAction(void *gridBlock, int param1, float param2, float param3)` | confidence: high | clear x87 floating point usage, SSE conversion via __ftol2_sse, clamping logic, and loop over 64 iterations calling UpdateGridCellRegion | source: llm_

**Performs coordinate scaling, clamped grid indexing, and a loop-driven region update on a 2D grid/terrain structure using floating-point math and trigonometric lookup tables.**

The function begins by taking input coordinates and scaling them using constants from .rdata, converting them via __ftol2_sse into integer grid indices (clamped between 0 and 95, or 0x5F). It accesses a byte element within a grid structure (offset 0x9304) and sets flag bits (or 3) at a calculated offset within the grid block. It then enters a loop running 64 times (0 to 63), performing trigonometric or geometric transformations using tables at `0x421818` and `0x42181C`, converting float values to integer indices via `__ftol2_sse`, and finally invoking `UpdateGridCellRegion` with the computed bounds, grid pointer, and loop parameters.

**Parameters:**
- `gridBlock` (void*, [esp+24h+arg_0]) - Pointer to the main resource / grid block structure.
- `param1` (int, [esp+24h+arg_4]) - An integer identifier or configuration index used in grid offset calculations.
- `param2` (float, float on stack / FPU) - Floating-point coordinate or scale input.
- `param3` (float, float on stack / FPU) - Floating-point coordinate or scale input.

**Returns:** none

**Side effects:**
- Modifies bitflags within the grid block data structures at calculated offsets (+4).
- Calls UpdateGridCellRegion to modify and look up elements in a 96x96 grid/terrain region.

**Called by:**
- `UpdateSimulationGridRegions` (0x004036B0) - call at 0x0040374C  
  Called inside the inner item-processing loop for elements that pass status, flag, and bitmask checks. It receives stack arguments derived from the current element's properties and offset references from the main resource block (esi), executing a subsystem operation for the validated resource/simulation item.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00417B6F, 0x00417BBA, 0x00417BEB, 0x00417C76, 0x00417CBB
- `UpdateGridCellRegion` (0x00417A70) - call at 0x00417D4B

### UpdateGridCellRegion - `0x00417A70`

_original name `sub_417A70` | `void __cdecl UpdateGridCellRegion(void *gridBlock, float param1, float param2, float param3, float param4, int count, int limit)` | confidence: medium, complex x87 floating-point instruction sequence makes exact parameter typing approximate, but grid bounds (0x60 = 96) and array indexing math are clear. | source: llm_

**Performs bounds-checked modifications and lookups on a 2D grid/terrain array (96x96 space) based on floating-point coordinate transformations and loop parameters.**

The function takes a grid-related data structure pointer and several parameters including floating-point coordinates and integer limits. It iterates a specified number of times (controlled by 'count'), using x87 FPU calculations and __ftol2_sse to compute 2D integer grid indices (esi and eax). It verifies that these indices fall within a 96x96 grid boundary ([0, 96)). For valid grid coordinates, it sets bits in a bitfield/flag byte within the grid cell structure (e.g., ORing by 3). If a condition flag (bl) is zero, it performs additional checks against structure offsets (such as comparing a byte at offset 0x9304 against arg_18 and checking bit 3 at offset 0xB704). If any check fails or bounds are exceeded, the loop terminates early or skips the update.

**Parameters:**
- `gridBlock` (void *, [esp+4 + arg_0]) - Pointer to the base grid/terrain block structure.
- `param1` (float, x87 FPU stack / stack arguments) - Floating-point coordinate or parameter used in grid transformation.
- `param2` (float, stack arguments) - Floating-point coordinate or parameter.
- `param3` (float, stack arguments) - Floating-point coordinate or parameter.
- `param4` (float, stack arguments) - Floating-point coordinate or parameter.
- `count` (int, [esp+4 + arg_14]) - Number of iterations for the inner grid processing loop.
- `limit` (int, [esp+4 + arg_18]) - Threshold or limit value compared against grid cell data.

**Returns:** none

**Side effects:**
- Modifies grid cell flags and attributes within the passed grid structure based on calculated 96x96 grid coordinates.

**Called by:**
- `ProcessGridRegionAction` (0x00417B50) - call at 0x00417D4B  
  Called repeatedly inside a loop that runs 64 iterations (edi from 0 to 0x3F), passing calculated integer coordinates, resource/grid block pointers, and computed parameters to perform a sub-operation or render/update step for grid cells.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00417AD4, 0x00417AE1

### VM_ExecuteEntityEventScript - `0x0040A0D0`

_original name `sub_40A0D0` | `bool __fastcall VM_ExecuteEntityEventScript(void *vm_context, void *unused, int event_index)` | confidence: high | complete register, flow, and callee correlation identified | source: llm_

**Validates and executes a virtual machine bytecode block associated with a specific entity event, managing VM state backup and restoration around the execution.**

The function first performs bounds and state validation on the VM context structure passed in esi and the event index passed in ebx. It checks if the VM's active flag (`[esi+8]`) is non-zero and whether the event index is within valid bounds (`0 <= ebx < [esi+0x90]`). If validation fails, it immediately returns false. When valid, it resets or backs up specific VM state variables (such as counts and pointers at offsets 0xA64, 0x998, and 0x99C) into local stack storage. It then loops a number of times determined by a byte at `[esi+ebx*4+0x12]`, pushing argument structures onto a temporary stack area and copying them into the VM's parameter/argument buffers (managed via the count at `[esi+0x998]`, ensuring it does not exceed 0x100). Afterward, it invokes sub_409940 [VM_ExecuteBytecodeBlock] to execute the bytecode script corresponding to the event index. Finally, it restores the previous VM state registers, checks the execution result and status flag (`[esi+0A68h]`), and returns true if execution succeeded without overflow or error.

**Parameters:**
- `vm_context` (void *, ecx) - Pointer to the main VM / entity context structure (this pointer for __fastcall).
- `event_index` (int, ebx) - The index of the entity event or bytecode script to execute.

**Returns:** bool (in al), true on successful execution and false otherwise

**Side effects:**
- modifies VM state and argument buffers within the context structure at esi
- invokes sub_409940 [VM_ExecuteBytecodeBlock]

**Data structures:**
- VM Context structure: +0x8 int active_flag, +0x12 array of event byte limits/codes, +0x90 int max_events, +0x198 parameter/stack buffer array, +0x998 int param_count, +0x99C int param_context_val, +0xA64 int vm_counter/offset, +0xA68 byte overflow/error flag

**Recursion:** part of a recursive cycle with `VM_ExecuteBytecodeBlock`, `ScheduleTimedEventOrState`, `EnqueueStateEntry`, `DispatchEntityEvent`

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401F44  
  Called after successful validation checks to execute a primary execution or teardown phase for the subsystem.
- `DispatchEntityEvent` (0x0040A1E0) - call at 0x0040A20A  
  Called conditionally when the input index is within bounds and the flag byte at [esi + eax*4 + 0x9A2] is non-zero, passing an extracted byte value (from offset +1 of a calculated record pointer) to perform an element-specific operation or dispatch.

**Calls:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x0040A198

### VM_ExecuteBytecodeBlock - `0x00409940`

_original name `sub_409940` | `int __fastcall VM_ExecuteBytecodeBlock(void *vm_context, void *unused, int bytecode_index, int base_offset)` | confidence: high | complete identification of VM interpreter loop, opcode dispatch table, and stack operations. | source: llm_

**Executes a block of virtual machine bytecode instructions within a given context, managing operand stacks, local variables, flow control, and dispatching built-in opcode handlers.**

The function processes bytecode sequences from a virtual machine state structure (in ESI). It validates the starting bytecode index and recursion/execution limits before entering a main processing loop. Inside the loop, it fetches opcode bytes, uses a lookup table (indexed via byte_40A060 and a jump table) to route execution to specific handling routines or inline logic. These operations include pushing/popping operands (`VM_PushValue`, `VM_PopOperand`), performing binary math (`ExecuteBinaryOp`), comparing values (`CompareVariantValues`), checking operand truth states, and dispatching to a large vtable of specialized opcode handlers (slots 0 through 29). It also supports recursive calls to itself for nested execution blocks, maintains tracking counters, and handles error states by setting failure flags in the VM context.

**Parameters:**
- `vm_context` (void *, ecx) - Pointer to the virtual machine state/context structure (this pointer via __fastcall).
- `bytecode_index` (int, [esp+124h+arg_0]) - Index of the bytecode block/resource to execute.
- `base_offset` (int, [esp+124h+arg_4]) - Base offset or frame parameter used for indexing local variables and stack allocations.

**Returns:** Returns 0 or positive on success, or -1 (0xFFFFFFFF) on error/failure in EAX.

**Side effects:**
- Modifies the virtual machine's stack and state tracking structures (at offsets +998h, +99Ch, +0A64h, +0A68h, etc.).
- Pushes and pops values on the VM operand stack.
- Invokes various opcode handlers and recursively calls itself.

**Data structures:**
- VMContext: +0x8 bytecode buffer pointer, +0xC bytecode size limit, +0x90 max index limit, +0x998 stack count, +0x99C recursion/block depth, +0xA64 instruction counter, +0xA68 error flag.

**Recursion:** calls itself; part of a recursive cycle with `VM_ExecuteEntityEventScript`, `ScheduleTimedEventOrState`, `EnqueueStateEntry`, `DispatchEntityEvent`

**Called by:**
- `VM_ExecuteEntityEventScript` (0x0040A0D0) - call at 0x0040A198  
  Called after gathering and staging items from the resource state structure (ESI) into internal buffers. It receives the current index (EBX) and a saved count value (on the stack), with 'this' pointer in ESI via __thiscall convention. Its return value in EAX (and internal flag checks) determines whether this function reports success (al = 1) or failure (al = 0).

**Calls:**
- `VM_PushValue` (0x004098B0) - call at 0x00409A8B, 0x00409ADC, 0x00409AF9, 0x00409B37, 0x00409BB7, 0x00409C01, 0x00409C2E, 0x00409C89 (+9 more)
- `VM_PopOperand` (0x004098F0) - call at 0x00409B16, 0x00409B4A, 0x00409B81, 0x00409B8C, 0x00409BC7, 0x00409C3E, 0x00409C49, 0x00409C99 (+11 more)
- `ExecuteBinaryOp` (0x004094B0) - call at 0x00409B9B
- `CompareVariantValues` (0x00409680) - call at 0x00409C60
- `CheckOperandTruthState` (0x00409480) - call at 0x00409CAB, 0x00409CB8, 0x00409D18, 0x00409D25, 0x00409D75, 0x00409DC5
- `OpcodeHandler_LoadConstantOrVariable` (0x0040A2A0) - call at 0x00409ECA; vtable entry: slot 0 of vtable 0x0041D228, vtable used at 0x00409EAC
- `OpcodeRandomRange` (0x0040A2D0) - call at 0x00409ECA; vtable entry: slot 1 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteRandomRangeOp` (0x0040A380) - call at 0x00409ECA; vtable entry: slot 2 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteSpawnEntityScriptOpcode` (0x0040A3F0) - call at 0x00409ECA; vtable entry: slot 3 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ResolveScriptReference` (0x0040A540) - call at 0x00409ECA; vtable entry: slot 4 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VM_GetGameVariableFloat` (0x0040A5A0) - call at 0x00409ECA; vtable entry: slot 5 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteScriptOpcode_TypeCheckOrRangeOp` (0x0040A620) - call at 0x00409ECA; vtable entry: slot 6 of vtable 0x0041D228, vtable used at 0x00409EAC
- `BytecodeOpcodeHandler_Slot7` (0x0040A6E0) - call at 0x00409ECA; vtable entry: slot 7 of vtable 0x0041D228, vtable used at 0x00409EAC
- `BytecodeOpcode_Slot8_EvalOrDefault` (0x0040A720) - call at 0x00409ECA; vtable entry: slot 8 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VM_ResolveResourceHandle` (0x0040A760) - call at 0x00409ECA; vtable entry: slot 9 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteBytecodeOp_10` (0x0040A7E0) - call at 0x00409ECA; vtable entry: slot 10 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VM_OpcodeHandler_Slot11` (0x0040A830) - call at 0x00409ECA; vtable entry: slot 11 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteBytecodeTypeOp` (0x0040A880) - call at 0x00409ECA; vtable entry: slot 12 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteBytecodeOpcode13` (0x0040A8E0) - call at 0x00409ECA; vtable entry: slot 13 of vtable 0x0041D228, vtable used at 0x00409EAC
- `EvalScriptReference` (0x0040A960) - call at 0x00409ECA; vtable entry: slot 14 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ScriptOp_GetResourceValue` (0x0040A9E0) - call at 0x00409ECA; vtable entry: slot 15 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteEntityInteractionOrCommandOpcode` (0x0040AA60) - call at 0x00409ECA; vtable entry: slot 16 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteBytecodeVtableSlot17` (0x0040AB80) - call at 0x00409ECA; vtable entry: slot 17 of vtable 0x0041D228, vtable used at 0x00409EAC
- `HandleBytecodeStatusOrEvent` (0x0040ABE0) - call at 0x00409ECA; vtable entry: slot 18 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteOpcode19` (0x0040AC10) - call at 0x00409ECA; vtable entry: slot 19 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteScriptMathOrConversionOp` (0x0040AC70) - call at 0x00409ECA; vtable entry: slot 20 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ScheduleTimedEventOrState` (0x0040AD00) - call at 0x00409ECA; vtable entry: slot 21 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VM_OpcodeHandler_Slot22` (0x0040ADF0) - call at 0x00409ECA; vtable entry: slot 22 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteScriptOpcode_Slot23` (0x0040AE80) - call at 0x00409ECA; vtable entry: slot 23 of vtable 0x0041D228, vtable used at 0x00409EAC
- `ExecuteVmOpcodeSlot24` (0x0040AEF0) - call at 0x00409ECA; vtable entry: slot 24 of vtable 0x0041D228, vtable used at 0x00409EAC
- `BytecodeOpcode_HandleReferenceOrResource` (0x0040AF60) - call at 0x00409ECA; vtable entry: slot 25 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VerifyObjectReference` (0x0040AFE0) - call at 0x00409ECA; vtable entry: slot 26 of vtable 0x0041D228, vtable used at 0x00409EAC
- `VirtualOpcodeHandler_Slot27` (0x0040B060) - call at 0x00409ECA; vtable entry: slot 27 of vtable 0x0041D228, vtable used at 0x00409EAC
- `GetSystemStateOrValue` (0x0040B090) - call at 0x00409ECA; vtable entry: slot 28 of vtable 0x0041D228, vtable used at 0x00409EAC
- `HandleOpcodeType2` (0x0040B0C0) - call at 0x00409ECA; vtable entry: slot 29 of vtable 0x0041D228, vtable used at 0x00409EAC
- `GetFloatOperand` (0x00409460) - call at 0x00409F25
- `__ftol2_sse` (0x0041A160) - call at 0x00409F6A

### GetFloatOperand - `0x00409460`

_original name `sub_409460` | `double __cdecl GetFloatOperand(const void *operand)` | confidence: high | The function directly matches the caller description for extracting/converting a float operand based on a type tag in [eax]. | source: llm_

**Extracts and converts a floating-point operand value based on its type tag, returning it as a double in st(0).**

The function checks a type tag byte at the address passed in EAX. If the type byte equals 2, it loads a floating-point value from the operand structure, normalizes/reloads it via temporary floating-point stack operations, and leaves the final float in st(0). Otherwise (if the type is not 2, typically representing an integer type), it loads the value as an integer using fild, converts it, and leaves the resulting float in st(0).

**Parameters:**
- `operand` (const void *, eax) - Pointer to an operand structure containing a type tag byte at offset 0 followed by the value data.

**Returns:** st(0) (floating-point value)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409F25  
  Called to convert or extract a floating-point value from an operand buffer (passed in EAX with type indicator 2 in byte_100), returning the float in st(0).

### HandleOpcodeType2 - `0x0040B0C0`

_original name `sub_40B0C0` | `void __cdecl HandleOpcodeType2(void *this_ptr, const void *operand_struct)` | confidence: high | complete disassembly mapped accurately to structure fields and global offsets | source: llm_

**Handles a specific bytecode/script operand or instruction by checking its type tag and storing either a converted floating-point or direct integer value into a global state structure.**

The function checks the type tag byte at the start of the operand structure passed via `[esp+4]`. If the type byte equals 2, it loads the floating-point value from the structure, converts it to a 32-bit integer using `__ftol2_sse`, and stores the integer into a global buffer at an offset (`dword_42AC60 + 0x5BBD4`), while setting a status flag at `dword_42AC60 + 0x5BBD0` to 1. If the type tag is not 2, it reads an integer value directly from offset +4 of the operand structure and performs the same store and flag update operations.

**Parameters:**
- `this_ptr` (void*, ecx / implicit) - The 'this' pointer for the current interpreter/script object (passed via ESI from caller, though not explicitly referenced except via global state).
- `operand_struct` (void*, [esp+4]) - Pointer to an operand structure containing a type tag byte at offset 0 and a value (float or int) at offset 4.

**Returns:** none

**Side effects:**
- modifies a global state structure at dword_42AC60 (+0x5BBD0 and +0x5BBD4)

**Data structures:**
- Operand structure: +0x0 byte type tag, +0x4 value (float or int)
- Global state object at dword_42AC60: +0x5BBD0 status byte, +0x5BBD4 integer value storage

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 29 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 29 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040B0CC

### GetSystemStateOrValue - `0x0040B090`

_original name `sub_40B090` | `void __cdecl sub_40B090(void *this_ptr, int arg4, int arg8, void *out_struct)` | confidence: high, straightforward assembly reading from a global variable and populating an output buffer. | source: llm_

**Retrieves a global state value and writes a 64-bit output structure containing a boolean flag and the retrieved value.**

The function reads a pointer from global variable `dword_42AC60`, accesses a dword field at offset +0x5BBBCh of that structure, and stores it in the second dword of a destination structure passed via the argument list. It also sets the first dword of the destination structure to 1.

**Parameters:**
- `this_ptr` (void*, esp+4) - Object instance pointer (this)
- `arg4` (int, esp+8) - First additional argument
- `arg8` (int, esp+12) - Second additional argument
- `out_struct` (int*, esp+16) - Pointer to an output structure receiving a byte/int flag (set to 1) and a retrieved global dword

**Returns:** none

**Side effects:**
- writes a 64-bit structure to the memory location pointed to by out_struct

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 28 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 28 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### VirtualOpcodeHandler_Slot27 - `0x0040B060`

_original name `sub_40B060` | `void __stdcall VirtualOpcodeHandler_Slot27(void *this_ptr, int arg4, int arg8, int *out_status)` | confidence: medium, disassembled snippet is slightly truncated or has raw fld/fstp instructions without explicit operands, but overall stack parameter usage and output structure writing are clear. | source: llm_

**Executes a specific script/bytecode virtual opcode method from vtable slot 27, writing a status code and an integer value to an output structure.**

The function fetches a global pointer or value from 0x0042AC60, interacts with the floating-point stack, and writes a status integer (value 2) and another local variable/parameter into an output structure pointed to by the `out_status` argument (passed at stack offset `arg_C`). It cleans up its local stack allocation and returns normally.

**Parameters:**
- `this_ptr` (void*, esi (via caller convention / object instance)) - The object instance pointer (this)
- `arg_4` (int, [esp+4]) - First operand or argument
- `arg_8` (int, [esp+8]) - Second operand or argument
- `out_status` (int*, [esp+12]) - Pointer to output structure where status/result fields are written

**Returns:** none

**Side effects:**
- Writes an integer value '2' and another local value into the memory structure pointed to by the output argument.

**Data structures:**
- Output structure: +0x0 int status/type (set to 2), +0x4 int value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 27 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 27 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### VerifyObjectReference - `0x0040AFE0`

_original name `sub_40AFE0` | `__cdecl int VerifyObjectReference(void *this_ptr, void *arg1, int arg2, int *out_status)` | confidence: high, clean register analysis and straightforward validation checks | source: llm_

**Validates a structured object/resource reference from bytecode operands against a global registry table and returns a status flag.**

The function inspects an argument structure (passed at esp+arg_4), verifying that its initial byte is 3 and that its secondary pointer field is non-zero. It then extracts an index from the lower 16 bits of the identifier and a validation tag from the upper 16 bits. Using a global manager table located at 0x0042AC60, it bounds-checks the index and retrieves a pointer from an internal array, verifying that the stored tag matches the upper 16 bits. Finally, it traverses the retrieved object pointer to check internal flags (byte at offset +1AHi equals 1, and bit 0 of the byte at +1Ch offset shifted right is set), returning 1 if all checks pass or 0 otherwise. The final result is written as a status output structure to the pointer specified in arg_C.

**Parameters:**
- `this_ptr` (void*, [esp+0]) - Object instance pointer (this)
- `obj_ref` (void*, [esp+4]) - Pointer to operand structure containing the resource/object reference
- `arg2` (int, [esp+8]) - Additional argument (unused or count)
- `out_status` (int*, [esp+12]) - Pointer to output status structure receiving result code

**Returns:** Returns 1 in al if validation succeeds, or 0 if it fails

**Side effects:**
- Writes a result status structure (type code 4 and boolean flag) to the address provided in out_status

**Data structures:**
- Global table at 0x0042AC60: contains an array pointer at +51E80h and a count at +51E84h

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 26 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 26 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### BytecodeOpcode_HandleReferenceOrResource - `0x0040AF60`

_original name `sub_40AF60` | `int __cdecl BytecodeOpcode_HandleReferenceOrResource(void *this_ptr, void *operand, int count, void *output_result)` | confidence: high, clear assembly logic matching standard game-engine handle/resource resolution patterns | source: llm_

**Resolves a packed resource/reference identifier from a bytecode operand, validates it against a global manager table and object state, invokes a virtual method on the target object, and writes a success/failure status and result value to an output structure.**

The function begins by inspecting the input operand structure (passed at `[esp+arg_4]`). It checks that the type byte at offset 0 equals 3 and that the pointer/identifier at offset +4 is non-zero. It then extracts a lower 16-bit index (`ecx & 0xFFFF - 1`) and looks up a global manager table located at `dword_42AC60`. It verifies that the index is within bounds (`[edx+51E84h]`). Using the base array at `[edx+51E80h]`, it checks if the upper 16-bits of the identifier (`ecx >> 16`) match a version or generation tag stored at offset +4 of the looked-up entry (`[eax+4]`). If valid, it fetches the associated object pointer from offset 0, ensures it is non-existent/non-null, verifies a byte flag at `[ecx+1Ah] == 1`, and finally performs an indirect virtual call via the object's vtable (`[eax+24h]`). If all checks and the virtual call succeed, it sets a result status byte to 1 and stores the virtual call's return value (in EAX) alongside it into the output pointer structure (`arg_C`). If any validation check fails at any stage, the function falls through to an error path that writes a status of 1 with a return value of 0.

**Parameters:**
- `this_ptr` (void*, [esp+arg_0]) - The 'this' pointer for the bytecode interpreter or execution context (passed in ESI by caller).
- `operand` (void*, [esp+arg_4]) - Pointer to the bytecode operand structure containing the reference type and packed resource/object ID.
- `count` (int, [esp+arg_8]) - Count or secondary argument passed via the dispatch table.
- `output_result` (void*, [esp+arg_C]) - Pointer to an output structure where the execution status and resulting value/handle are written.

**Returns:** eax (or 0 on failure, or the return value of the invoked virtual method on success)

**Side effects:**
- reads from global manager table at 0x0042AC60
- invokes a virtual method via an object's vtable at offset +0x24
- writes a status byte and return value to the output structure passed in arg_C

**Data structures:**
- Global Manager Table: +0x51E80 pointer to element array, +0x51E84 element count
- Manager Array Element: +0x0 object pointer, +0x24 virtual method offset
- Output Result Structure: +0x0 status byte/word, +0x4 result value/pointer

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 25 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 25 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ExecuteVmOpcodeSlot24 - `0x0040AEF0`

_original name `sub_40AEF0` | `void __stdcall ExecuteVmOpcodeSlot24(void *this_ptr, const void *operandPtr, int count, void *outputStruct)` | confidence: medium, based on standard VM interpreter operand processing and calling conventions for virtual methods. | source: llm_

**Executes a virtual machine opcode handler (vtable slot 24) that processes operand types, converts values using floating-point operations or type checks, and stores a formatted result into an output structure.**

The function begins by inspecting the variant/operand structure pointed to by the second argument. If its type tag byte equals 2, it loads the floating-point value and converts it to an integer using `__ftol2_sse`; otherwise, it reads a 32-bit integer field from offset +4 of the operand structure. It then performs a bounds check (less than or equal to 5), scaling the value by a global factor stored at `0x0042AC60` if within bounds, or multiplying/loading alternative constant float data if above 5. Finally, it stores the resulting components into the output structure provided via the fourth argument.

**Parameters:**
- `this_ptr` (void*, ecx) - The 'this' pointer for the virtual method.
- `operandPtr` (const void*, [esp+4]) - Pointer to an operand structure or variant input.
- `count` (int, [esp+8]) - Count or secondary parameter.
- `outputStruct` (void*, [esp+12]) - Pointer to an output structure where the processed result is written.

**Returns:** none

**Side effects:**
- Writes computed values to the output structure pointed to by the fourth parameter.

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 24 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 24 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AEFF

### ExecuteScriptOpcode_Slot23 - `0x0040AE80`

_original name `sub_40AE80` | `void __cdecl ExecuteScriptOpcode_Slot23(void *pThis, const void *pOperand1, int count, void *pOutput)` | confidence: medium, based on assembly control flow and virtual method context in bytecode execution. | source: llm_

**Executes a virtual opcode handler (vtable slot 23) in the bytecode interpreter, fetching and converting operand data based on type flags to perform a math or conversion operation returning a floating-point/variant value.**

The function inspects an operand structure passed via arguments. It checks the type flag at offset 0 of the first operand pointer. If the type is 2, it converts an x87 floating-point value to a 32-bit integer via __ftol2_sse; otherwise, it reads a 32-bit integer from offset 4. It then compares this integer against a maximum limit (5); if within bounds, it scales the value using a global factor from `dword_42AC60`. If out of bounds, it loads a constant float and multiplies it. Finally, it formats the resulting value and copies it into the output structure buffer provided via arguments.

**Parameters:**
- `pThis` (void*, ecx / implicit this or first arg depending on call convention) - The 'this' pointer for the interpreter object.
- `pOperand1` (const void*, [esp+4]) - Pointer to the input operand structure/variant.
- `count` (int, [esp+8]) - Count or secondary argument for the operation.
- `pOutput` (void*, [esp+0xC]) - Pointer to the output destination buffer where the resulting variant is written.

**Returns:** none

**Side effects:**
- Writes computed result/variant data to the output pointer specified by the caller.

**Data structures:**
- Operand/Variant: +0x0 byte/int type tag, +0x4 integer or float payload value.

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 23 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 23 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AE8F

### VM_OpcodeHandler_Slot22 - `0x0040ADF0`

_original name `sub_40ADF0` | `void __fastcall VM_OpcodeHandler_Slot22(void *pVMState, void *pArg1, int count, void *pResultStruct)` | confidence: high | medium confidence based on consistent bytecode/VM handler patterns and field usage. | source: llm_

**Executes a virtual machine opcode handler (vtable slot 22) that reads an operand, converts or extracts an integer value, checks bounds against an array limit, and appends or registers an item into a VM state queue or buffer.**

The function begins by inspecting the type of an operand passed via argument (arg_4). If the operand type is 2, it loads a floating-point value and converts it to a 32-bit signed integer using __ftol2_sse. Otherwise, it extracts an integer value from a secondary offset within the operand structure. It then checks the VM state object's current count at offset +0xA60: if it has reached or exceeded 16 (0x10), or if the extracted integer is negative or greater than or equal to the array limit stored at offset +0x90, it fails and returns an invalid result status (writing 1 and -1 to the output structure). If bounds checks pass, it increments the count at +0xA60, calculates an index into an array of structures starting at +0x9A0 (each entry being 12 bytes: 3 dwords or similar layout), populates the entry with flags and the extracted integer value, and writes success indicators to the output status struct.

**Parameters:**
- `pVMState` (void*, ecx) - Pointer to the virtual machine state or context structure.
- `pArg1` (void*, [esp+4]) - First operand or parameter structure pointer.
- `count` (int, [esp+8]) - Count or secondary argument (unused or context-dependent).
- `pResultStruct` (void*, [esp+0C]) - Pointer to an output structure receiving the operation status and index/handle.

**Returns:** none

**Side effects:**
- Modifies VM state fields including the count at +0xA60 and array entries starting at +0x9A0
- Writes result status and index/handle to the output structure passed in pResultStruct

**Data structures:**
- VMState: +0x90 int max_limit, +0xA60 int active_count, +0x9A0 array of items (12 bytes each: +0 flag, +1 byte value, etc.)
- Operand: +0 type, +4 value or nested pointer

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 22 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 22 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040ADFF

### ScheduleTimedEventOrState - `0x0040AD00`

_original name `sub_40AD00` | `void __cdecl ScheduleTimedEventOrState(void *this_ptr, const void *param1, int param2, void *status_out)` | confidence: high | based on clear caller vtable integration, state queue manipulation, and call to EnqueueStateEntry | source: llm_

**Evaluates a timed/parameterized script value, checks against scheduling limits, and enqueues a new state or event entry via EnqueueStateEntry.**

The function inspects a variant-like input parameter structure to extract a floating-point value and a secondary integer or float component. It clamps or validates the extracted index against a limit stored at offset +0x90 and a maximum active entry count of 0x10 stored at offset +0xA60 of the primary 'this' object. If valid, a new entry is populated in an internal fixed-size array/queue structure inside the object (each entry sized at 12 bytes, based on the lea stride pattern `ebp + ebp*2`), initialized with status bytes, and passed to EnqueueStateEntry (sub_419280). Finally, it populates the output status struct with the resulting entry index or -1 on failure.

**Parameters:**
- `this_ptr` (void *, [esp+4] / arg_0) - The object instance pointer (containing state queues and limits).
- `param1` (const void *, [esp+8] / arg_4) - Input descriptor/variant structure containing opcode/type fields and data.
- `arg2` (int, [esp+12] / arg_8) - Additional numeric argument or context flag.
- `status_out` (int *, [esp+16] / arg_C) - Output structure populated with status and allocated entry index (or -1).

**Returns:** none

**Side effects:**
- modifies the state container queue inside the object
- calls EnqueueStateEntry to enqueue the new element
- updates the active entry count at offset +0xA60

**Data structures:**
- Object: +0x90 max limit index, +0xA60 active count, +0x9A0 start of state/event entry array

**Recursion:** part of a recursive cycle with `VM_ExecuteEntityEventScript`, `VM_ExecuteBytecodeBlock`, `EnqueueStateEntry`, `DispatchEntityEvent`

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 21 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 21 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AD3E
- `EnqueueStateEntry` (0x00419280) - call at 0x0040ADAA

### EnqueueStateEntry - `0x00419280`

_original name `sub_419280` | `int __stdcall EnqueueStateEntry(void *state_container, int arg1, int arg2, int arg3, int arg4)` | confidence: high | based on clear structural patterns matching container growth, heap insertion, and field initialization. | source: llm_

**Allocates or inserts a new element into a state/priority queue container, resizes the container if capacity is exceeded, populates the new entry with provided data and flags, and restores heap order using a sift-up routine.**

The function operates on a state container structure passed via `esi`. It first increments a counter at `[esi+0Ch]`. It then compares the current count (`[esi+4]`) with the capacity limit (`[esi+8]`). If capacity is reached or exceeded, it calls `ResizeStateCapacity` (`sub_419800`) to grow the buffer. Afterward, it computes the memory address for the new entry using a stride calculation (`[esi+4] * 24` or similar structure size), populates the slot with arguments retrieved from the stack (`var_18`, `var_14`, a function pointer to `DispatchEntityEvent`, etc.), and increments the element count `[esi+4]`. Finally, it calls `HeapUp` (`sub_419590`) passing the updated index to restore binary heap ordering.

**Parameters:**
- `state_container` (void*, esi) - Pointer to the state and priority queue container structure.
- `data1` (int, stack (esp+20h + var_18)) - First field value to populate in the new entry.
- `data2` (int, stack (esp+20h + var_14)) - Second field value to populate in the new entry.
- `data3` (int, stack (esp+20h + arg_8)) - Fourth field value to populate in the new entry.
- `data4` (int, stack (esp+20h + arg_C)) - Fifth field value to populate in the new entry.

**Returns:** Returns the old value of `[esi+0Ch]` in eax.

**Side effects:**
- Modifies the state container pointed to by esi (increments counts, writes to element buffer)
- May reallocate the container's internal buffer via ResizeStateCapacity
- Calls HeapUp to adjust the priority queue structure

**Data structures:**
- StateContainer: +0x0 pointer/data, +0x4 current element count, +0x8 capacity limit, +0x0Ch counter/index

**Recursion:** part of a recursive cycle with `VM_ExecuteEntityEventScript`, `VM_ExecuteBytecodeBlock`, `ScheduleTimedEventOrState`, `DispatchEntityEvent`

**Called by:**
- `ScheduleTimedEventOrState` (0x0040AD00) - call at 0x0040ADAA  
  Called as part of the state recording/allocation routine after storing operational flags and index details into a newly claimed slot within the runtime's state array (pointed to by EDI), passing context to update or initialize the newly created entry.

**Calls:**
- `DispatchEntityEvent` (0x0040A1E0) - address taken at 0x004192A7
- `ResizeStateCapacity` (0x00419800) - call at 0x004192B2
- `sub_419590` (0x00419590) - call at 0x004192EE

### sub_419590 - `0x00419590`

_source: llm (unparsed response)_

**{ "suggested_name": "HeapUp", "prototype": "void __fastcall HeapUp(int element_index, void *collection_structure)", "short_summary": "Performs a 'heap-up' (sift-up / percolate-up) operation on a binary heap or priority queue, bubbling an element up toward the root as long as its priority/key is greater than its parent's.", "detailed_summary": "The function implements standard binary heap up-heap maintenance. It takes an index (passed in eax) and a structure pointer (passed via stack argument `arg_0`). It enters a loop where it calculates the parent index (`edi = (index - 1) / 2`), scal [...]**

{
  "suggested_name": "HeapUp",
  "prototype": "void __fastcall HeapUp(int element_index, void *collection_structure)",
  "short_summary": "Performs a 'heap-up' (sift-up / percolate-up) operation on a binary heap or priority queue, bubbling an element up toward the root as long as its priority/key is greater than its parent's.",
  "detailed_summary": "The function implements standard binary heap up-heap maintenance. It takes an index (passed in eax) and a structure pointer (passed via stack argument `arg_0`). It enters a loop where it calculates the parent index (`edi = (index - 1) / 2`), scales both child and parent indices to match the 24-byte (0x18) element stride (using multiplications and shift/lea sequences), and compares their keys (stored at offset +0x14 of each element). Floating-point comparisons are also performed depending on the code path (handling potential float priorities or secondary comparisons). If the child node is greater than or equal to its parent, the loop terminates; otherwise, it swaps the elements between the child and parent slots, updates the index to the parent index, and repeats the process upward until reaching the root (when `edi <= 0`).",
  "parameters": [
    {
      "name": "element_index",
      "location": "eax",
      "type": "int",
      - "description": "The index of the newly added or modified element to bubble up."
    },
    {
      "name": "collection_structure",
      "location": "[esp+4]",
      "type": "void*",
      "description": "Pointer to the priority queue / heap container structure."
    }
  ],
  "return_value": "none",
  "side_effects": [
    "reorders elements in the heap array stored within the collection structure"
  ],
  "data_structures": [
    "Heap element: 24 bytes (0x18) in size, with key/priority stored at offset +0x14."
  ],
  "confidence": "high | standard binary heap up-heap / sift-up algorithm operating on an array with 24-byte elements and a key at offset +0x14."
}

**Called by:**
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x0041956F  
  Performs heap-up or maintenance operations on a specific element index (passed in eax) within the collection structure (passed on the stack via ebx) after a new item is inserted.
- `EnqueueStateEntry` (0x00419280) - call at 0x004192EE  
  Called with ESI pushed on the stack after successfully writing the new record and incrementing the count, passing the updated index (count minus one) to notify or re-index the execution state.

### ResizeStateCapacity - `0x00419800`

_original name `sub_419800` | `void* __fastcall ResizeStateCapacity(void *context_struct, size_t requested_capacity)` | confidence: high, clear logic mirroring standard vector/dynamic array growth routines with explicit stride multiplication (48 bytes per entry). | source: llm_

**Grows the capacity and reallocates the underlying data buffer of a state/allocation context object to accommodate a new required size.**

The function calculates a new buffer capacity based on the current capacity stored in the context structure (at offset +8) and the requested minimum capacity passed in ECX. If the current capacity is zero, it initializes a base capacity of 0x10. Otherwise, it doubles the capacity repeatedly in a loop until it meets or exceeds the requested capacity in ECX. It then computes the new byte size by multiplying the new capacity by 48 (via a series of lea and add instructions: `edi = ebx * 48`), and calls ReallocateCustomBlock to resize the memory block pointed to by the buffer pointer at offset +0 of the context. Finally, it updates the context structure with the new buffer pointer (`[ebp+0] = eax`) and the new capacity (`[ebp+8] = ebx`), returning the new buffer pointer.

**Parameters:**
- `context_struct` (void*, stack [esp+8+arg_0]) - Pointer to the state/allocation context structure holding the buffer pointer and capacity limits.
- `requested_capacity` (size_t, ecx) - The minimum capacity required (current capacity + 1).

**Returns:** void* in eax, pointing to the newly allocated or reallocated memory block.

**Side effects:**
- Reallocates memory via ReallocateCustomBlock, potentially freeing the old buffer and invalidating previous pointers.
- Updates capacity and buffer pointer fields in the passed context structure.

**Data structures:**
- ContextStruct: +0x0 void* buffer_ptr, +0x4 int count, +0x8 int capacity

**Called by:**
- `EnqueueStateEntry` (0x00419280) - call at 0x004192B2  
  Called with ESI pushed on the stack and ECX set to current capacity plus one when the current count of elements ([esi+4]) equals the capacity limit ([esi+8]), presumably to grow or reallocate the underlying storage buffer.

**Calls:**
- `ReallocateCustomBlock` (0x00418670) - call at 0x00419832

### DispatchEntityEvent - `0x0040A1E0`

_original name `sub_40A1E0` | `void __cdecl DispatchEntityEvent(void *context, int index)` | confidence: high, clean assembly logic with clear bounds checking, indexing, and callback dispatch. | source: llm_

**Checks if an index is within bounds and an element flag is active, then extracts a byte identifier from the element and dispatches it to sub_40A0D0.**

The function takes a context pointer and an integer index. It first verifies whether the index is strictly less than the capacity limit stored at offset +0xA60 of the context structure. If out of bounds, it returns immediately. If valid, it calculates a table offset using scaled index arithmetic (`index * 12`), checks a flag byte located at context + index*12 + 0x9A2, and returns if this flag is zero. Otherwise, it extracts a byte value from offset +1 of the record (context + index*12 + 0x9A0 + 1), preserves `ebx`, and invokes `sub_40A0D0` passing the extracted byte.

**Parameters:**
- `context` (void*, [esp+arg_0]) - Pointer to the main execution context or manager structure.
- `index` (int, [esp+arg_4]) - Index of the entry/element to process.

**Returns:** none

**Side effects:**
- Calls sub_40A0D0 conditionally based on element flags and index bounds.

**Data structures:**
- Context/Manager: +0xA60 int capacity, +0x9A0 start of element records (each record is 12 bytes: +0x0/2 flags/data, +1 byte value passed to sub_40A0D0).

**Recursion:** part of a recursive cycle with `VM_ExecuteEntityEventScript`, `VM_ExecuteBytecodeBlock`, `ScheduleTimedEventOrState`, `EnqueueStateEntry`

**Called by:**
- `EnqueueStateEntry` (0x00419280) - address taken at 0x004192A7  
  The function's address is loaded into EBX and stored directly into the third dword offset (+8) of the newly initialized entry structure within the internal array. It is stored as a callback or handler reference for that record.

**Calls:**
- `VM_ExecuteEntityEventScript` (0x0040A0D0) - call at 0x0040A20A

### ExecuteScriptMathOrConversionOp - `0x0040AC70`

_original name `sub_40AC70` | `void __cdecl ExecuteScriptMathOrConversionOp(void *thisPtr, void *scriptVarInput, int unused, void *scriptVarOutput)` | confidence: medium, based on the vtable slot usage description, argument structure, and calls to CreateScriptVariable and ftol2_sse. | source: llm_

**Executes a virtual script opcode that performs math, type conversion, or variable creation on input script values, returning a newly allocated script variable result.**

The function acts as a virtual opcode handler for script interpretation (slot 20 of a vtable). It inspects the input script variable structure (`scriptVarInput`, passed at `arg_4`), checks type tags (e.g., comparing against `bl = 2`), converts floating-point values to integers using `__ftol2_sse` when necessary, manipulates values using x87 FPU instructions, and calls `CreateScriptVariable` to instantiate a resulting script variable. Finally, it stores the output type and value into the destination script variable structure pointer (`arg_C`).

**Parameters:**
- `thisPtr` (void*, ecx (or implicit this)) - Pointer to the script execution context or interpreter object (this).
- `scriptVarInput` (void*, [ebp+arg_4]) - Pointer to the input script variable structure.
- `unused` (int, [ebp+arg_8]) - Additional parameter or count (part of the 4-argument vtable dispatch).
- `scriptVarOutput` (void*, [ebp+arg_C]) - Pointer to the output script variable structure where the result is written.

**Returns:** none

**Side effects:**
- Allocates a new script variable via CreateScriptVariable
- Writes the resulting type and value to the scriptVarOutput structure

**Data structures:**
- ScriptVariable: +0x0 type tag (byte/word), +0x4 value or data pointer, +0x8 secondary field, +0x10 type/flag field

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 20 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 20 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AC87
- `CreateScriptVariable` (0x004027A0) - call at 0x0040ACCD

### CreateScriptVariable - `0x004027A0`

_original name `sub_4027A0` | `void* __stdcall CreateScriptVariable(char typeCode)` | confidence: high | based on clear instantiation, initialization, and container management pattern common to game script runtimes. | source: llm_

**Allocates and initializes a new script variable object of a specified type, registers it within a global runtime container pool, and returns a pointer to the created variable object.**

The function first allocates a 36-byte (0x24) custom memory block using AllocateCustomBlock. If successful, it passes the allocated memory and a type code parameter to ScriptVariable_Initialize to initialize the script variable object (setting up its vtable and type-specific properties). It then accesses a global manager structure at dword_42AC60, checks or updates a free-list or pool allocator tracking structure for variable objects, and links the newly created variable into a container/array managed by that subsystem (growing the container via GrowContainerCapacity if necessary). If initialization or allocation fails, or if container capacity limits trigger cleanup, it handles destruction or returns null appropriately.

**Parameters:**
- `typeCode` (char, [esp+8]) - The type code or identifier used to initialize the script variable.

**Returns:** Returns a pointer to the newly allocated and initialized script variable object (in eax), or 0 on failure.

**Side effects:**
- Allocates memory via AllocateCustomBlock
- May resize a global container/array via GrowContainerCapacity
- Modifies global state at dword_42AC60 and related container fields

**Called by:**
- `ExecuteScriptMathOrConversionOp` (0x0040AC70) - call at 0x0040ACCD  
  Called with parameters prepared from processed script operand values (including floating-point/integer conversions loaded onto the x87 stack and an integer code pushed on the stack) to perform a core operation, returning a pointer whose field is used to update the output status structure.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004027AD
- `ScriptVariable_Initialize` (0x00410BD0) - call at 0x004027BA
- `GrowContainerCapacity` (0x00404100) - call at 0x00402833

### ScriptVariable_Initialize - `0x00410BD0`

_original name `sub_410BD0` | `void* __fastcall ScriptVariable_Initialize(void *varObj, int unused, char typeCode)` | confidence: high based on clear initialization patterns, field offsets, and caller context. | source: llm_

**Initializes a script variable or runtime property object with default values, a vtable pointer, and a type-specific property computed from a passed-in identifier or type flag.**

The function takes an allocated object pointer in `eax` and a byte/char value in `cl` (via `__fastcall`-like convention or caller setup), along with two dword arguments on the stack (`[esp+4]` and `[esp+8]`). It sets up the object's virtual table pointer at offset 0 to `off_41D3FC`, initializes various internal fields (such as setting offsets +4, +8, +0Ch, +18h, +20h), and populates floating-point fields using `fld1`, `fldz`, and a constant from `.rdata`. It performs a conditional operation on the type parameter `cl` (checking if `cl >= 0Eh`), computes an index via arithmetic and a lookup table (`byte_41C9E2`), and stores the resulting byte at offset +1Dh, while saving `cl` at offset +1Ch.

**Parameters:**
- `varObj` (void*, eax) - Pointer to the newly allocated script variable or runtime object to initialize.
- `typeCode` (char, ecx) - Type code or specifier passed to configure the variable/property.
- `arg1` (int, esp+4) - Auxiliary argument stored into the object at offset +8.
- `arg2` (int, esp+8) - Auxiliary argument stored into the object at offset +0Ch.

**Returns:** Returns the initialized object pointer in eax.

**Side effects:**
- Initializes the memory block pointed to by eax as a script variable/object structure.

**Data structures:**
- ScriptVariable: +0x0 vtable*, +0x4 dword, +0x8 int/ptr, +0x0Ch int/ptr, +0x18h word, +0x1Ah byte, +0x1Bh byte, +0x1Ch byte (typeCode), +0x1Dh byte (lookup result), +0x20h dword

**Called by:**
- `CreateScriptVariable` (0x004027A0) - call at 0x004027BA  
  Called if the initial check succeeds, taking an argument from the function's parameter stack (`arg_0`) in `ecx` to construct or initialize the newly allocated object, returning the resulting object pointer in `eax`.

### ExecuteOpcode19 - `0x0040AC10`

_original name `sub_40AC10` | `__thiscall void ExecuteOpcode19(void *this, int *arg1, int arg2, int *outStatus)` | confidence: high | standard vtable override pattern with clear operand parsing and table lookup | source: llm_

**Implements a bytecode virtual machine instruction handler (vtable slot 19) that checks type and value flags against a global constraint table.**

The function receives a pointer to a value/operand structure (arg1). It checks if its type byte is 2; if so, it loads a floating-point value from the structure and converts it to an integer using __ftol2_sse; otherwise, it fetches an integer value from offset +4 of the structure. It then performs a bounds and bitfield check against a global data table (dword_42AC60) scaled by the integer value, using a bitmask of 4. Finally, it stores a result flag derived from this check into the output status pointer (arg_C).

**Parameters:**
- `this` (void*, ecx) - The VM / interpreter instance pointer (__thiscall convention)
- `operand` (int*, [esp+4]) - Pointer to an operand structure containing a type byte and value
- `count` (int, [esp+8]) - Unused or auxiliary count argument
- `outStatus` (int*, [esp+12]) - Pointer to output buffer receiving status values (writes 4 to [ecx] and a boolean flag to [ecx+4])

**Returns:** none

**Side effects:**
- Writes computed status values into the output structure pointed to by arg_C

**Data structures:**
- Operand structure: +0x0 type byte, +0x4 integer value (or float at +0x0)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 19 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 19 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AC1F

### HandleBytecodeStatusOrEvent - `0x0040ABE0`

_original name `sub_40ABE0` | `void __cdecl HandleBytecodeStatusOrEvent(int a1, const void *operand)` | confidence: high, clear control flow and register usage mapped cleanly to known callers and callees | source: llm_

**Extracts an integer value or type code from a bytecode operand structure and uses it as an index to trigger event handlers if it falls within a valid range (0-5).**

The function inspects an operand structure passed via stack argument. It checks the byte at offset 0 of the operand: if it equals 2, it loads a floating-point value from the structure, converts it to a 32-bit integer using __ftol2_sse; otherwise, it reads a 32-bit integer from offset 4 of the operand structure. Next, it checks if this extracted integer value is less than or equal to 5 (unsigned comparison). If within this range [0, 5], it preserves EBX, moves the integer into EBX, and calls sub_403BF0 [TriggerHandlersByIndex] with the index in EBX to trigger related game/script handlers.

**Parameters:**
- `this_pointer` (void*, esp+0) - Unused or 'this' context passed via vtable dispatch (ESI)
- `operand` (const void*, esp+4) - Pointer to a bytecode operand structure

**Returns:** none

**Side effects:**
- triggers global event handlers by index via sub_403BF0

**Data structures:**
- Bytecode operand structure: +0x0 type byte, +0x4 integer/float value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 18 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 18 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040ABEC
- `TriggerHandlersByIndex` (0x00403BF0) - call at 0x0040ABFE

### TriggerHandlersByIndex - `0x00403BF0`

_original name `sub_403BF0` | `void __fastcall TriggerHandlersByIndex(int index)` | confidence: high based on clear register usage, pointer arithmetic, loops, and virtual method call patterns. | source: llm_

**Performs a squared-index lookup into a global data structure to flag a status bit and iterates through an array of registered objects, invoking a virtual method on any object whose identifier matches the input index and whose state conditions are met.**

The function takes an integer index passed in EBX. It first computes the square of this index (`eax = ebx * ebx`), adds it to a base pointer retrieved from the global `dword_42AC60`, and checks a status byte at offset +3 of that computed address. If the flag (bit 2) is already set, it returns immediately. Otherwise, it sets bit 2 at that location. Then, it iterates through a list of objects stored in an array located at `[esi + 0x5738C]`, with the count given at `[esi + 0x57390]`. For each object pointer, it checks if a byte field at offset +0x1B matches the input index EBX, and checks that bit 0 of the inverted byte at offset +0x18 is set (indicating an active/enabled status). If both conditions are met, it retrieves a virtual function pointer from the object's vtable (`*(vtable + 0x0C)`) and invokes it with a parameter of 0.

**Parameters:**
- `index` (int, ebx) - The operand or index value processed from bytecode (bounded to <= 5).

**Returns:** none

**Side effects:**
- Modifies a flag byte within a global table derived from dword_42AC60 based on the square of the input index.
- Iterates over a list of registered runtime objects and invokes a virtual function method on matching items.

**Data structures:**
- Global struct at dword_42AC60 containing lookup flags/tables at squared offsets.
- Runtime object list array at base + 0x5738C with count at base + 0x57390.
- Runtime object: +0x0 vtable, +0x18 state/flags byte, +0x1B identifier/index byte.

**Called by:**
- `HandleBytecodeStatusOrEvent` (0x0040ABE0) - call at 0x0040ABFE  
  Called with the processed integer value (clamped/bounded to be <= 5) in EBX to perform the core operation or lookup associated with this opcode handler.

### ExecuteBytecodeVtableSlot17 - `0x0040AB80`

_original name `sub_40AB80` | `int __cdecl ExecuteBytecodeVtableSlot17(void *this_ptr, int arg1, int arg2, int *out_status)` | confidence: high, clear control flow, standard calling conventions, and well-mapped callee interactions. | source: llm_

**Executes a virtual opcode handler that evaluates a type-checked operand (converting floats to integers or reading integer fields), computes a table lookup index, and stores a status flag and result.**

The function inspects an argument structure passed at `arg_4`. If its type tag byte at offset 0 equals 2, it loads a floating-point value and converts it to a 32-bit integer via `__ftol2_sse`; otherwise, it reads an integer value from offset +4 of the structure. It then checks if the resulting value is within the range [0, 5]. If out of range, it writes a status flag of 1 and an output value of 0 to the output structure pointer (`arg_C`). If in range, it multiplies the value by itself (squaring it), adds a base constant from `dword_42AC60`, calls `LookupBytecodeTable` (`sub_4044D0`), and writes a status flag of 1 along with the table lookup result into `arg_C`.

**Parameters:**
- `this_ptr` (void*, ecx (implied by vtable context)) - Object instance pointer (this)
- `arg_4` (int*, [esp+0x4]) - Pointer to an input operand structure
- `arg_8` (int, [esp+0x8]) - Additional bytecode interpreter argument or count
- `out_status` (int*, [esp+0xC]) - Pointer to an output status/result structure where status (offset +0) and result (offset +4) are written

**Returns:** none

**Side effects:**
- Writes status and result values to the output structure pointed to by arg_C

**Data structures:**
- Input Operand Struct: +0x0 type byte, +0x4 integer value / float storage
- Output Struct: +0x0 status byte/int, +0x4 result int

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 17 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 17 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040AB8F
- `LookupBytecodeTable` (0x004044D0) - call at 0x0040ABAC

### LookupBytecodeTable - `0x004044D0`

_original name `sub_4044D0` | `int __fastcall LookupBytecodeTable(int unused, int struct_ptr)` | confidence: high | clean loop structure inspecting a static table in .rdata and accumulating values from a structure pointer | source: llm_

**Iterates through a hardcoded table in the data section, conditionally accumulating 16-bit field values from a provided structure based on bit flags.**

The function performs a loop over a table of 39 entries (progressing through 39 iterations from byte_41B178 up to the string 'dPF<2' at 0x0041BCF4 with a stride of 0x3C bytes). In each iteration, it checks a bit flag located at the current table pointer. If the low bit is set, it extracts two 16-bit values from offsets [edx] and [edx+0x62] of the input structure pointer passed in edx (which is pre-adjusted by adding 0x14), adds them together along with an accumulating sum, and updates the sum in eax. The input structure pointer (edx) is incremented by 2 on each iteration.

**Parameters:**
- `edx_struct_ptr` (void*, edx) - Pointer to a data structure (accessed with offsets like +0x14, +0x16, +0x76, etc. due to the loop increments).

**Returns:** Accumulated integer result in eax

**Called by:**
- `ExecuteBytecodeVtableSlot17` (0x0040AB80) - call at 0x0040ABAC  
  Called after computing a scaled index (value squared plus a global base address from dword_42AC60), presumably to perform an underlying data lookup or retrieval operation whose integer return value is placed into the output structure.

### ExecuteEntityInteractionOrCommandOpcode - `0x0040AA60`

_original name `sub_40AA60` | `void __fastcall ExecuteEntityInteractionOrCommandOpcode(void *this_ptr, void *unused, const void *operand_struct, void *status_output)` | confidence: high, clear operand parsing and validation routine leading directly into ProcessEntityInteractionOrCommand. | source: llm_

**Executes a specialized entity interaction or command bytecode instruction, validating an entity reference and marshaling arguments into a parameter structure before invoking ProcessEntityInteractionOrCommand.**

The function begins by checking the type and validity of the primary operand structure passed in edx. It looks up a global entity or resource table using dword_42AC60, verifying index and version/handle validity to retrieve an entity pointer. If validation fails, it populates the status output struct with an error/failure status code (4) and returns. If valid, it extracts several values from the operand structure (handling floating-point conversions and integer loads depending on type flags), constructs a local parameter structure on the stack containing these parsed arguments, and passes them along with the entity pointer to ProcessEntityInteractionOrCommand (sub_40DAC0). Finally, it writes the result status from the command handler back to the status output structure and returns.

**Parameters:**
- `this_ptr` (void*, ecx) - The 'this' pointer for the bytecode execution context (passed via fastcall).
- `unused` (void*, edx) - Unused ecx/edx fastcall slot (or secondary parameter depending on convention).
- `operand_struct` (const void*, [ebp+8] (arg_4)) - Pointer to the bytecode operand structure containing entity handles and command parameters.
- `status_output` (void*, [ebp+12] (arg_C)) - Pointer to an output status structure receiving execution result flags.

**Returns:** none

**Side effects:**
- Modifies the status output structure provided by the caller.
- Interacts with global entity tables and triggers entity interactions or commands via ProcessEntityInteractionOrCommand.

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 16 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 16 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040AB60

### ScriptOp_GetResourceValue - `0x0040A9E0`

_original name `sub_40A9E0` | `void __cdecl ScriptOp_GetResourceValue(void *pThis, void *pArg1, int arg2, void *pResultStruct)` | confidence: high, clear operand validation logic, global table lookups, and structured result output pattern typical of game scripting engines. | source: llm_

**Evaluates a resource/object identifier operand from bytecode, bounds-checking it against a global resource manager table, and returns a floating-point or typed property value into a result structure.**

The function inspects an operand structure passed via `arg_4`. It first verifies that the operand type byte equals 3 and that its secondary pointer/ID field is non-zero. It then extracts a 16-bit resource index and a 16-bit handle/version tag, checking the index against a global resource manager table located via `0x0042AC60`. If bounds and version tags match, it retrieves the resource pointer, checks an internal type flag at offset `+0x1A`, and loads a value (defaulting to zero if any check fails). Finally, it writes out a typed result structure to the pointer specified by `arg_C` (`[esp+8+arg_C]`).

**Parameters:**
- `pThis` (void*, ecx / [esp+arg_0]) - This pointer for the virtual method call (interpreter instance).
- `pOperand` (void*, [esp+arg_4]) - Pointer to the bytecode operand structure being evaluated.
- `unk_arg2` (int, [esp+arg_8]) - Additional argument or count passed to the opcode handler.
- `pResultStruct` (void*, [esp+arg_C]) - Pointer to the output structure where the evaluated result (type + value) is stored.

**Returns:** none (result is written via output pointer; floating point operations use st(0) temporarily)

**Side effects:**
- Reads global resource manager state at 0x0042AC60
- Writes a typed value structure to the caller-provided output pointer

**Data structures:**
- Operand structure: +0x0 byte type, +0x4 dword identifier/packed handle (low word: index, high word: version/tag)
- Global resource manager table at 0x0042AC60: +0x51E80 pointer to resource entries, +0x51E84 integer count
- Resource entry: +0x0 pointer to resource object, +0x4 word version/tag
- Resource object: +0x1A byte flag

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 15 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 15 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### EvalScriptReference - `0x0040A960`

_original name `sub_40A960` | `void __cdecl EvalScriptReference(const void *arg0, int arg4, int arg8, void *outResult)` | confidence: high, clear operand validation, global table bounds checking, and output writing sequence. | source: llm_

**Evaluates a script or bytecode reference operand (checking type tag 3, index bounds, and validity) to produce a floating-point value written to an output buffer.**

The function begins by checking if the input pointer (arg_4) starts with a type tag of 3. If so, it extracts a 32-bit identifier from offset +4, performs bounds checking against a global table stored at 0x0042AC60 (verifying the table index and a 16-bit version/discriminator tag in the high word), and looks up a pointer in an array of structures. If any check fails or the target object lacks a specific flag (offset +1A == 1), it falls back to loading 0.0 onto the x87 floating-point stack. Otherwise, it loads a floating-point value (or reference) from the resolved object. Finally, it stores the resulting value and associated metadata into the output structure provided via arg_C.

**Parameters:**
- `operand` (const void*, [esp+4]) - Pointer to the script operand or bytecode structure containing a type tag and reference identifier.
- `arg2` (int, [esp+8]) - Unused or secondary argument slot.
- `outResult` (void*, [esp+0xC]) - Pointer to an output buffer where the resulting evaluated value and type tag are written.

**Returns:** none

**Side effects:**
- Writes evaluation results (floating-point value and type indicator) to the memory location specified by the output parameter.

**Data structures:**
- `Global Lookup Table (0x0042AC60)`

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 14 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 14 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ExecuteBytecodeOpcode13 - `0x0040A8E0`

_original name `sub_40A8E0` | `void __cdecl ExecuteBytecodeOpcode13(void *pThis, int a2, void *pArgs, int *pStatus)` | confidence: high, clear validation logic, direct indexing into global registry tables, and straightforward output population. | source: llm_

**Executes a specific bytecode instruction handler (vtable slot 13) that validates a resource ID and type, writing status and resulting data values back to the output structure.**

The function inspects an argument structure passed via pointer (`arg_4`). It first verifies that the byte at offset 0 equals 3 and that a pointer at offset +4 is non-zero. It then extracts an index from the lower 16 bits of this pointer and a version/handle check value from the upper 16 bits. Using a global data structure registry at `0x0042AC60`, it validates that the index is within bounds and that the stored version/handle matches the upper 16 bits of the identifier. It further checks that the referenced object is active (`byte ptr [eax+1Ah] == 1`). If all validation checks pass, it retrieves a byte from offset +5C of the object, sets a success/active status flag (1) in the output pointer (`pStatus`), and writes the retrieved byte value. If any validation check fails, it writes a failure/default status (1, with the secondary value set to -1 / 0xFFFFFFFF).

**Parameters:**
- `pThis` (void*, [esp+4]) - The object instance pointer (this pointer) for the bytecode execution context.
- `arg_4` (void*, [esp+8]) - Pointer to the input operand structure containing bytecode parameter data.
- `pArgs` (void*, [esp+0xC]) - Additional argument or context pointer.
- `pStatus` (int*, [esp+0x10]) - Output structure pointer where status flags and resulting values are written (status at +0, value at +4).

**Returns:** none

**Side effects:**
- reads from global data registry at 0x0042AC60
- writes status and result data to the output pointer supplied in arguments

**Data structures:**
- Global registry at 0x0042AC60: +0x51E80 pointer to resource array, +0x51E84 integer count
- Resource descriptor entry: +0x0 pointer to target object, +0x2 unsigned short version/check value
- Target object: +0x1A byte state flag, +0x5C byte property value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 13 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 13 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ExecuteBytecodeTypeOp - `0x0040A880`

_original name `sub_40A880` | `void __fastcall sub_40A880(void *this_ptr, int dummy, const void *operand_ptr, int count, int *output_struct)` | confidence: high, clear logic mapping operand tags, table lookups, and output population consistent with a bytecode VM instruction handler | source: llm_

**Executes a virtual machine bytecode opcode handler that checks a type-tagged operand, extracts or converts its value, performs a lookup in a global table, and writes a status/result pair to an output structure.**

The function receives an operand pointer and an output structure pointer. It inspects the type tag at the first byte of the operand structure: if the tag is 2, it loads a floating-point value from the operand and converts it to a 32-bit signed integer via __ftol2_sse; otherwise, it reads an integer value from offset +4 of the operand. It then bounds-checks this integer value (must be <= 5). If within bounds, it uses it as an index to fetch an entry from a table located via the global pointer at 0x0042AC60 (using a multi-byte element scaling via imul). If the index is out of bounds (> 5), the resulting value defaults to 0. Finally, it writes a success status byte (value 1) to the first 4 bytes (or word/byte) of the output structure and the resulting value to the subsequent 4 bytes.

**Parameters:**
- `this_ptr` (void*, ecx) - The VM / interpreter 'this' pointer (passed via __fastcall conventions in the vtable chain).
- `operand_ptr` (const void*, [esp+4]) - Pointer to an input operand or value structure, where the first byte is a type tag.
- `count` (int, [esp+8]) - Count or secondary argument passed through the vtable dispatcher.
- `output_struct` (int*, [esp+12]) - Pointer to an output structure where a status flag (1) is written at offset +0 and a computed/looked-up result is written at offset +4.

**Returns:** none

**Side effects:**
- writes a status flag (1) and a computed result integer into the caller-provided output structure pointer

**Data structures:**
- Operand structure: +0x0 byte type tag, +0x4 int value (or float at +0x4 if type == 2)
- Output structure: +0x0 int status flag, +0x4 int result value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 12 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 12 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040A88F

### VM_OpcodeHandler_Slot11 - `0x0040A830`

_original name `sub_40A830` | `void __fastcall VM_OpcodeHandler_Slot11(void *pThis, void *edx_unused, void *arg_struct)` | confidence: high | standard VM opcode handler pattern with type coercion and bounds checking | source: llm_

**Executes a virtual machine bytecode instruction handler that processes typed operands (converting floating-point to integer if necessary) and updates a global state array based on an opcode index.**

The function begins by checking the type tag at the start of an argument structure (pointed to by arg_4). If the tag is 2, it loads a floating-point value from the x87 stack and converts it to an integer using __ftol2_sse; otherwise, it reads a 32-bit integer directly from offset +4 of the argument structure. This value is used as an index (ESI) bounded between 0 and 5 inclusive. It then fetches a second operand: if the type tag at offset +8 is 2, it converts another floating-point value from the x87 stack; otherwise, it reads a 32-bit integer from offset +0Ch. The index is multiplied by a value from a global base array located at 0x0042AC60, and the resulting offset is updated by adding the second operand value.

**Parameters:**
- `pThis` (void*, ecx) - The 'this' pointer for the virtual table object (implicit fastcall)
- `arg_struct` (void*, [esp+8]) - Pointer to an argument/operand structure containing type tags and values

**Returns:** none

**Side effects:**
- modifies values in the global data structure rooted at 0x0042AC60

**Data structures:**
- Argument struct: +0x0 byte type tag, +0x4 int/float value, +0x8 byte type tag, +0x0C int/float value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 11 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 11 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040A83E, 0x0040A858

### ExecuteBytecodeOp_10 - `0x0040A7E0`

_original name `sub_40A7E0` | `void __fastcall ExecuteBytecodeOp_10(void *this, void *edx, void *arg1)` | confidence: high | disassembly is straightforward and aligns with the caller's vtable dispatch context | source: llm_

**Executes virtual opcode slot 10, performing type checks/conversions on operands (using __ftol2_sse) and updating state variables stored in a global data structure.**

The function inspects an operand structure passed via arg1. It checks the type tag at [edi]. If the tag is 2, it loads a floating-point value from the x87 stack and converts it to an integer using __ftol2_sse; otherwise, it reads an integer value from [edi+4]. It then performs a bounds check (ensuring the value is <= 5). Next, it checks a second operand at [edi+8]; if its tag is 2, it converts a floating-point value via __ftol2_sse, otherwise it reads an integer from [edi+0Ch]. It multiplies the first operand by a global multiplier stored at dword_42AC60, adds the second operand's value to an element within a global array/structure, and updates the reference.

**Parameters:**
- `this` (void*, ecx) - Virtual method 'this' pointer (bytecode interpreter context)
- `arg1` (void*, [esp+0x8]) - Pointer to operand structure/arguments for the opcode

**Returns:** none

**Side effects:**
- modifies global array/structure elements based on calculated indices and operands

**Data structures:**
- Operand structure: +0x0 type tag, +0x4 value or pointer, +0x8 second type tag, +0xC second value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 10 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 10 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040A7EE, 0x0040A808

### VM_ResolveResourceHandle - `0x0040A760`

_original name `sub_40A760` | `void __cdecl VM_ResolveResourceHandle(void *this_ptr, const void *operand, int count, int *out_status)` | confidence: high, clear validation logic and structured table lookup matching a typical bytecode virtual machine implementation | source: llm_

**Validates and resolves a resource/object handle from bytecode operand data, returning a status code and index/handle value.**

The function inspects an operand structure passed as an argument. It first checks that the type byte at offset 0 equals 3 and that the pointer/ID at offset +4 is non-zero. It then treats the lower 16 bits of that ID minus 1 as an index into a global resource/object table located via data pointer at 0x0042AC60. After bounds-checking this index against a table size limit, it retrieves a table entry, verifies that its upper 16-bit tag/version matches the upper 16 bits of the original ID, and checks an internal flag at offset +1A (expecting value 1). If all checks pass successfully, it extracts a byte value from offset +1B of the resolved resource object, writes a success indicator (1) to the status output pointer, and stores the extracted byte into the second dword of the status output. If any check fails, it writes a failure/invalid indicator (status = 1, value = 0xFFFFFFFF) to the output structure instead.

**Parameters:**
- `this_ptr` (void*, [esp+4]) - The VM or interpreter context 'this' pointer (passed via ESI in the vtable caller).
- `operand` (const void*, [esp+8]) - Pointer to the bytecode operand structure containing type and resource ID information.
- `count` (int, [esp+C]) - An argument/count associated with the instruction execution.
- `out_status` (int*, [esp+10]) - Pointer to an output structure (2 dwords) receiving the result status and resolved handle/value.

**Returns:** none

**Side effects:**
- writes success or failure status and resolved resource data into the output structure pointed to by out_status

**Data structures:**
- Global resource table manager at 0x0042AC60: contains array pointer at +0x51E80 and count limit at +0x51E84
- Resource table entry: +0x0 pointer to resource object, +0x2 word tag/version
- Resource object: +0x1A byte flag (checked for 1), +0x1B byte value (extracted on success)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 9 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 9 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### BytecodeOpcode_Slot8_EvalOrDefault - `0x0040A720`

_original name `sub_40A720` | `void __cdecl sub_40A720(void *this_ptr, void *arg1, void *arg2, int *out_status)` | confidence: high due to clear pointer navigation, null checking, and straightforward output struct population matching a vtable opcode handler. | source: llm_

**Evaluates a nested script/bytecode operand structure or retrieves a default value, writing a type tag and a value pointer into an output status structure.**

The function inspects an argument object passed via stack parameters, navigating a pointer offset at +4. If the resulting pointer is non-null, it extracts a nested value pointer from its +4 offset and writes a type tag of 3 alongside this value into an output structure pointed to by arg_C. If the initial pointer or its intermediate target is null/zero, it falls back to writing type tag 3 with a value of 0 into the output structure. This pattern matches the evaluation of script operands or variables in a bytecode interpreter context where an optional reference is dereferenced or defaulted.

**Parameters:**
- `this_ptr` (void*, [esp+0]) - The 'this' pointer for the virtual method (passed in ESI by caller).
- `arg1` (void*, [esp+4]) - First operand or resource structure pointer.
- `arg2` (void*, [esp+8]) - Second operand or auxiliary pointer.
- `out_status` (int*, [esp+12]) - Output status/result structure pointer receiving a type tag and a value.

**Returns:** none

**Side effects:**
- Writes a type tag (3) and a value or null pointer to the output structure passed at arg_C.

**Data structures:**
- Operand structure: +0x4 points to a nested structure or container
- Nested container: +0x4 holds a value pointer
- Output status structure: +0x0 int type tag (set to 3), +0x4 void* value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 8 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 8 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### BytecodeOpcodeHandler_Slot7 - `0x0040A6E0`

_original name `sub_40A6E0` | `void __stdcall BytecodeOpcodeHandler_Slot7(void *this_ptr, void *operand, int count, void *status_output)` | confidence: high | standard vtable override pattern handling bytecode VM instruction operand fetching and result formatting | source: llm_

**Executes a specific virtual bytecode instruction handler that dereferences an operand structure, checks its validity, and populates a status/result output structure with a type code and a secondary value.**

The function begins by retrieving the first argument (operand pointer) from the stack, dereferencing it, and checking if the resulting pointer is null. If it is null, it writes a status code of 3 to the first 4 bytes of the status output structure (`status_output`) and 0 to the next 4 bytes, then returns. If the pointer is not null, it fetches a secondary value from offset +4 of the inner structure, writes a status code of 3 to the first 4 bytes of `status_output`, and writes the fetched secondary value to the subsequent 4 bytes of `status_output` before returning.

**Parameters:**
- `this_ptr` (void*, ecx / esi) - The object instance pointer (this)
- `operand` (void**, [esp+4] / arg_0) - Pointer to an operand structure or resource
- `count` (int, [esp+8]) - Count or secondary parameter
- `status_output` (int*, [esp+0xC] / arg_C) - Pointer to a 2-dword output structure receiving a type tag and a value

**Returns:** none

**Side effects:**
- Writes bytecode execution results/status into the output structure provided via argument 4

**Data structures:**
- StatusOutput: +0x00 int type_or_status (set to 3), +0x04 int value

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 7 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 7 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ExecuteScriptOpcode_TypeCheckOrRangeOp - `0x0040A620`

_original name `sub_40A620` | `void __cdecl sub_40A620(void *this_ptr, const void *operand_struct)` | confidence: medium, bytecode VM dispatch handler involving complex object validation and FPU range operations. | source: llm_

**Executes a specialized bytecode operation or validation handler from a script dispatch vtable, validating an operand reference and performing floating-point range or comparison logic.**

The function begins by inspecting a passed operand structure pointer (at arg_4). It checks if the primary type byte at offset 0 equals 3. If so, it extracts a 32-bit value at offset 4, separating it into a low 16-bit index and high 16-bit handle/version. It looks up a global structure array via `d自分word_42AC60`, checks boundaries against count fields, and validates the high 16-bit word against a stored instance or handle field. It then verifies internal flags on the retrieved object pointer (`[ecx+1Ah] == 1` and `[ecx+18h]` negated has bit 0 set). Depending on another type field at `[esi+8]`, it loads a floating-point value using either `fld` or `fild`, performs a series of floating-point comparison checks (`fcom`, `fnstsw`, status testing) to clamp or test ranges, and cleans up the FPU stack before returning.

**Parameters:**
- `this_ptr` (void*, ecx (passed via thiscall convention or in register/stack depending on caller)) - The object instance pointer ('this') for the virtual opcode handler.
- `operand_struct` (const void*, [esp+8]) - Pointer to the bytecode operand structure containing type and value information.

**Returns:** none

**Data structures:**
- Operand structure: +0x0 byte type, +0x4 dword handle/index value, +0x8 byte secondary type

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 6 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 6 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### VM_GetGameVariableFloat - `0x0040A5A0`

_original name `sub_40A5A0` | `float __cdecl VM_GetGameVariableFloat(void *operand, void *unused, struct OutVariant *outVariant)` | confidence: high | complete x86 control flow and memory access pattern matched for a scripting VM variable lookup handler. | source: llm_

**Resolves a bytecode variable operand to a floating-point value from a global game state table, returning 0.0f if the reference is invalid.**

The function inspects a bytecode operand structure passed via arguments. It first checks if the operand type byte is 3 and that its associated pointer/identifier (at offset +4) is non-zero. It then extracts an index from the lower 16 bits of the identifier, bounds-checks it against a global array count located at `[dword_42AC60 + 51E84h]`, and uses an array pointer at `[dword_42AC60 + 51E80h]` to access an element of size 8 (`eax*8`). It verifies a 16-bit generation/type tag from the upper 16 bits of the identifier against the element's validation field at offset +4. If all validations succeed, it follows the element's primary pointer, verifies a byte flag at offset +1AHi equals 1, and loads a float value; otherwise, it defaults to 0.0f. The resulting floating-point value and a type tag are written into the output variant structure passed as the third parameter.

**Parameters:**
- `operand` (void*, [esp+4]) - Pointer to the bytecode operand structure being evaluated.
- `unused` (void*, [esp+8]) - Unused or secondary argument slot.
- `outVariant` (struct OutVariant*, [esp+12]) - Pointer to an output structure receiving the resulting type tag and value.

**Returns:** st(0) containing the resolved floating-point value (or 0.0f on failure)

**Side effects:**
- Writes a type tag (value 2) and the floating-point/data payload to the output variant pointer.

**Data structures:**
- GlobalManager: +0x51E80 array pointer, +0x51E84 element count
- ElementStruct: +0x0 pointer to variable object, +0x4 16-bit tag/validation
- VariableObject: +0x1A byte flag

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 5 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 5 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ResolveScriptReference - `0x0040A540`

_original name `sub_40A540` | `void __cdecl ResolveScriptReference(void *this_ptr, const void *operand_struct)` | confidence: high, clear pointer arithmetic, bitfields, and virtual method call pattern typical of bytecode engines. | source: llm_

**Validates and dereferences a script/bytecode resource identifier, performing bounds checking against a global table and invoking a virtual method on the resolved target object.**

The function inspects an operand structure passed via stack (`[esp+arg_4]`). First, it verifies that the byte at offset 0 of the structure equals 3. If so, it reads a 32-bit value at offset +4. It extracts a 16-bit index from the lower half and a 16-bit identifier from the upper half (via right shift by 16). It then bounds-checks the index against a global resource/object table stored in `dword_42AC60`. If valid, it retrieves the entry, compares the upper 16 bits with a stored identifier field, and ensures a pointer within the entry is non-null. Finally, it checks a byte flag at offset +0x1A of the target object to ensure it equals 1, extracts a vtable function pointer from offset +0xC of the object's vtable, and calls it with a zero argument.

**Parameters:**
- `this_ptr` (void*, esi (via caller context)) - Object instance pointer passed in esi from the virtual dispatch table.
- `operand_struct` (const void*, [esp+4]) - Pointer to the bytecode operand structure containing type tag and packed resource reference ID.

**Returns:** none

**Side effects:**
- Invokes a virtual method (vtable slot 3, offset +0x0C) on a resolved internal object if all validation checks pass.

**Data structures:**
- Operand structure: +0x0 byte type tag (expects 3), +0x4 dword packed resource identifier (low 16-bit index, high 16-bit version/id).
- Global resource table manager at dword_42AC60: +0x51E80 array pointer, +0x51E84 count.

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 4 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 4 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### ExecuteSpawnEntityScriptOpcode - `0x0040A3F0`

_original name `sub_40A3F0` | `void __stdcall ExecuteSpawnEntityScriptOpcode(void *this_ptr, void *script_args, int unused, void *out_status)` | confidence: high | complete bottom-up context mapping and direct library/helper matches | source: llm_

**Executes a bytecode script instruction that spawns and registers a game entity based on coordinates, type, and variant parameters extracted from the script operand block.**

The function processes a script operand structure (passed in ESI), evaluating up to four initial numeric operands (handling both floating-point and integer representations via type tags) to determine entity type and variant identifiers. It validates these identifiers against boundary limits and configuration tables. Next, it extracts and converts coordinate parameters, calls `ConvertAndClampCoordinate` to clamp them to the valid grid range, and uses `CheckGridAreaOrFindNearby` to locate an accessible placement position on the game map grid. If a valid position is found, it computes final coordinates and invokes `CreateAndRegisterResourceEntity` to instantiate the entity and register it into the game's spatial tracking and resource management systems. Finally, it populates the status output structure (`out_status`) with the resulting entity identifier or zero on failure.

**Parameters:**
- `this_ptr` (void*, [ebp+8] (passed via EDI/ESI dispatch mechanism)) - Virtual dispatch 'this' pointer (bytecode interpreter context).
- `script_args` (void*, [ebp+12]) - Pointer to the bytecode operand block containing type, variant, and coordinate values.
- `unused` (int, [ebp+16]) - Unused parameter slot.
- `out_status` (void*, [ebp+20]) - Pointer to output status structure where operation result (type and entity ID) is written.

**Returns:** none

**Side effects:**
- Allocates and registers a new game entity via CreateAndRegisterResourceEntity
- Modifies spatial grid and resource tracking structures
- Writes result status and entity ID into the caller-provided out_status structure

**Data structures:**
- Script operand block: +0x0 type tag / integer, +0x4/0xC numeric values, +0x10/0x18 coordinate parameters
- Output status structure: +0x0 status code/type (set to 3), +0x4 created entity ID pointer

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 3 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 3 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040A407, 0x0040A41C
- `ConvertAndClampCoordinate` (0x00414AA0) - call at 0x0040A487, 0x0040A497
- `CheckGridAreaOrFindNearby` (0x00414E40) - call at 0x0040A4BC
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040A509

### ExecuteRandomRangeOp - `0x0040A380`

_original name `sub_40A380` | `__thiscall void ExecuteRandomRangeOp(void *this, int *pOperandStruct, int count, int *pOutputStatus)` | confidence: medium, based on vtable dispatch role and x87/RNG invocation patterns | source: llm_

**Virtual opcode handler that evaluates operands (possibly as integer bounds or floating-point values), queries a pseudo-random number generator, and writes out a computed result.**

The function acts as a virtual opcode handler (vtable slot 2) in a scripting/bytecode interpreter. It receives a pointer to an operand structure, checks type tags within that structure to load either integer or floating-point values via x87 instructions (`fild`/`fld`), sets up state parameters involving a global RNG structure accessed via `dword_42AC60`, invokes `UpdateRngAndCompute` to obtain a random/computed floating-point value, and writes the output back to the provided output status or result pointer.

**Parameters:**
- `this` (void*, ecx) - Object instance pointer
- `pOperandStruct` (int*, [esp+4]) - Pointer to operand structure or descriptor
- `count` (int, [esp+8]) - Count or secondary argument
- `pOutputStatus` (int*, [esp+0xC]) - Pointer to output location for result/status

**Returns:** none

**Side effects:**
- Updates RNG state via sub_4188F0
- Writes computed results to the memory location pointed to by pOutputStatus

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 2 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 2 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `UpdateRngAndCompute` (0x004188F0) - call at 0x0040A3CC

### OpcodeRandomRange - `0x0040A2D0`

_original name `sub_40A2D0` | `void __cdecl OpcodeRandomRange(void *this_ptr, struct RangeArg *arg, int unused, int *out_status)` | confidence: high, clear operand parsing, bounds checking, PRNG logic, and output writing matching bytecode interpreter vtable handlers. | source: llm_

**Executes a bytecode range/random instruction by extracting low and high bounds (converting floats to ints if needed), generating a random integer within that range, and storing the output result.**

The function begins by inspecting a descriptor structure passed as its second argument (`arg`). It retrieves two values (lower and upper bounds). For each bound, it checks a type indicator byte at offset +8 (or +0); if the value is a floating-point type (indicated by the value 2), it loads the floating-point value and converts it to a 32-bit signed integer using `__ftol2_sse`. Otherwise, it reads the integer directly from the structure offset. Once both bounds (`esi` as lower/min, `ebx` as upper/max) are obtained, it compares them. If `esi > ebx`, it uses a linear congruential or xorshift-like PRNG seeded in a global state block (`dword_42AC60`) to generate a pseudo-random number, scales it based on the range size (`esi - ebx + 1`), adds the minimum bound, and stores the resulting random value. If the lower bound is not greater than the upper bound, it simply returns the upper bound or handles it directly. Finally, it writes an integer status flag (value 1) and the result value to an output structure pointer provided in the fourth argument.

**Parameters:**
- `this_ptr` (void *, [esp+10h]) - The VM or bytecode executor instance pointer (this in ESI context).
- `arg` (struct RangeArg *, [esp+14h]) - Argument structure containing operand descriptors for the range bounds.
- `unused` (int, [esp+18h]) - Unused or count parameter passed from dynamic dispatch.
- `out_status` (int *, [esp+1Ch]) - Pointer to output status/result structure where the execution outcome is written.

**Returns:** none

**Side effects:**
- modifies global pseudo-random number generator state at 0x0042AC60
- writes execution status and result value to the output structure pointer

**Data structures:**
- RangeArg: +0x0 type/flags for bound 1, +0x4 int value 1, +0x8 type/flags for bound 2, +0xC int/float value 2

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 1 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 1 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0040A2E3, 0x0040A2F7

### OpcodeHandler_LoadConstantOrVariable - `0x0040A2A0`

_original name `sub_40A2A0` | `void __cdecl sub_40A2A0(void *this_ptr, void *unknown1, void *unknown2, int *out_result)` | confidence: medium | Based on caller context and stack layout, though floating point instructions are partially abbreviated in the disassembly view. | source: llm_

**Executes a virtual machine instruction handler that retrieves a value (possibly a constant or variable) and writes an 8-byte typed value structure to the output destination.**

The function allocates 8 bytes on the stack, accesses a global data pointer at 0x0042AC60, loads/stores floating-point values via the x87 FPU stack, and writes a typed output structure to the pointer provided in the last argument. Specifically, it writes an integer tag or type identifier of 2 into the first 4 bytes of the destination structure and a local variable or register value into the next 4 bytes.

**Parameters:**
- `this_ptr` (void*, esi (via caller context)) - The VM / interpreter context pointer (this pointer).
- `arg_4` (void*, [esp+4]) - First operand or parameter pointer.
- `arg_8` (void*, [esp+8]) - Second operand or parameter pointer.
- `out_result` (int*, [esp+12]) - Pointer to an 8-byte output structure where the resulting typed value is written.

**Returns:** none

**Side effects:**
- Writes an 8-byte typed value structure (type tag 2 and a data value) to the memory address pointed to by out_result.

**Data structures:**
- Result structure: +0x0 int type_tag (set to 2), +0x4 int value_or_data

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409ECA; vtable entry: slot 0 of vtable 0x0041D228, vtable used at 0x00409EAC  
  Retrieved from slot 0 of vtable 0x0041D228 and called via a dynamic dispatch table with four arguments (this pointer in ESI, fetched operand pointers, count, and status output) to execute a specific virtual opcode handler.

### CheckOperandTruthState - `0x00409480`

_original name `sub_409480` | `bool __cdecl CheckOperandTruthState(const void *operand)` | confidence: high, straightforward disassembly with clear type-dispatch and comparison logic matching caller expectations. | source: llm_

**Evaluates an operand structure's truth state or validity flag, returning true or false based on its type and value.**

The function examines an operand pointer passed in EAX. It first checks the byte at offset 0 (`[eax]`). If this type tag equals 2, it performs a floating-point comparison with zero (`fldz`, `fcomp`, `fnstsw`, and testing status flags via `test ah, 0x44` and `jnp`), returning true if the floating-point value satisfies the condition (non-zero/ordered) and false otherwise. If the type tag is not 2, it checks a secondary field at offset 4 (`[eax+4]`), returning true if that integer/pointer field is non-zero, and false otherwise.

**Parameters:**
- `operand` (const void*, eax) - Pointer to an operand or value structure inspected by the script engine.

**Returns:** Boolean flag in al (1 for true, 0 for false)

**Data structures:**
- Operand structure: +0x0 byte type/tag, +0x4 dword value or secondary field

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409CAB, 0x00409CB8, 0x00409D18, 0x00409D25, 0x00409D75, 0x00409DC5  
  Called to check or test the validity/truth state of an operand buffer (passed in EAX), returning a boolean flag in AL.

### CompareVariantValues - `0x00409680`

_original name `sub_409680` | `bool __fastcall CompareVariantValues(const Variant *left, const Variant *right, int opCode)` | confidence: high, clear operand parsing, type tagging checks, FPU comparisons, and integer condition code dispatches match a standard script VM comparison routine. | source: llm_

**Compares two variant/operand buffers (left in ECX, right in EAX) using a relational operator specified by EDX, supporting both integer and floating-point comparisons and returning a boolean result in AL.**

The function evaluates a comparison between two typed variant structures. Each variant appears to store a type tag byte at offset 0 and a value at offset 4 (or accessed via FPU load operations). First, it checks if either variant is of a floating-point type (indicated by a type tag of 2). If so, it loads both values onto the x87 FPU stack (converting integer types via fild if necessary) and dispatches to floating-point comparison handlers based on the operator code. Otherwise, it extracts the 32-bit values from offset +4 of each variant structure and performs integer comparisons (less-than, less-or-equal, greater, greater-or-equal, equal, not-equal) using standard setcc instructions. The resulting condition flag is returned in AL.

**Parameters:**
- `left` (const Variant*, ecx) - Pointer to the left-hand side variant/operand structure.
- `right` (const Variant*, eax) - Pointer to the right-hand side variant/operand structure.
- `opCode` (int, edx) - The comparison operator code indicating less-than, equal, greater-than, etc.

**Returns:** Boolean comparison result (0 or 1) returned in al.

**Data structures:**
- `Variant`

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409C60  
  Called to perform a comparison or relational operation between two operand buffers (passed via ECX and EAX) with an operator code (4) in EDX, returning a condition flag in AL.

### ExecuteBinaryOp - `0x004094B0`

_original name `sub_4094B0` | `int __fastcall ExecuteBinaryOp(void *operandA, void *operandB, int opCode)` | confidence: high, clear operand tagging pattern, standard operator jump tables, and FPU/integer arithmetic branches. | source: llm_

**Performs binary arithmetic operations (add, subtract, multiply, divide, modulo) on two script/bytecode variable operands, handling both integer and floating-point types.**

The function evaluates a binary arithmetic operation specified by `opCode` (`EDX`) on two variant-like operand structures, `operandA` (`EAX`) and `operandB` (`ECX`). It first checks the type tags of both operands (`[eax]` and `[ecx]`). If either operand is tagged as a floating-point type (represented by the tag value 2), it promotes both operands to floating-point and uses the x87 FPU to perform the operation (addition, subtraction, multiplication, division, or modulo via `_CIfmod`). Otherwise, it extracts the 32-bit integer values from offset +4 of each operand and executes integer arithmetic (including checks for division by zero). The resulting value and a type tag are returned across registers.

**Parameters:**
- `operandA` (void*, eax) - Pointer to the left-hand operand structure (type tag at +0, value at +4).
- `operandB` (void*, ecx) - Pointer to the right-hand operand structure (type tag at +0, value at +4).
- `opCode` (int, edx) - Operation code determining which arithmetic operation to perform.

**Returns:** Returns a combined result structure/value via registers (type tag / status in eax, result value in edx or floating-point stack).

**Data structures:**
- Variant Operand: +0x0 byte type tag (2 = float, others = integer), +0x4 union/value (int or float depending on tag)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409B9B  
  Called to perform a binary or arithmetic operation on two fetched variable/operand buffers (passed via ECX and EAX) with an operation code in EDX, returning a combined result.

**Calls:**
- `_CIfmod` (0x0041A21E) - call at 0x00409631

### _CIfmod - `0x0041A21E`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!_CIfmod.**

A single `jmp ds:__imp__CIfmod` through the import address table entry for MSVCR80!_CIfmod - calling this function is exactly calling _CIfmod.

**Called by:**
- `ExecuteBinaryOp` (0x004094B0) - call at 0x00409631

### VM_PopOperand - `0x004098F0`

_original name `sub_4098F0` | `bool __fastcall VM_PopOperand(VMContext *vm, Variant *outValue)` | confidence: high, clear stack pop logic with underflow checking and 8-byte element copying. | source: llm_

**Pops an 8-byte value (variant/operand) from the virtual machine's internal stack structure, returning false if the stack is underflowed or true on success.**

The function checks a stack pointer field at offset +0x998 of the VM state structure (passed in EAX). If this value is less than or equal to 0, it indicates a stack underflow; it sets a flag at offset +0xA68 to 1, writes a zero/default value (0) into the output buffer at ECX, and returns false (AL = 0). If the stack pointer is valid, it decrements the pointer, uses it to index into an array of 8-byte stack elements located at offset +0x198 of the VM structure, copies the 8-byte value into the output buffer (ECX), and returns true (AL = 1).

**Parameters:**
- `vm` (VMContext *, eax) - Pointer to the virtual machine state structure.
- `outValue` (Variant *, ecx) - Pointer to a caller-allocated 8-byte buffer where the popped value will be stored.

**Returns:** bool (in al): true (1) if an operand was successfully popped, false (0) on stack underflow.

**Side effects:**
- Decrements the VM stack pointer at offset +0x998 (if greater than 0)
- Sets an error/underflow flag at offset +0xA68 of the VM structure if underflow occurs
- Copies an 8-byte value from the VM stack into the buffer pointed to by ECX

**Data structures:**
- VMContext: +0x198 array of 8-byte stack elements, +0x998 stack pointer/count, +0xA68 underflow/error flag
- Variant: 8-byte structure containing a value/type representation (+0x0 dword, +0x4 dword)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409B16, 0x00409B4A, 0x00409B81, 0x00409B8C, 0x00409BC7, 0x00409C3E, 0x00409C49, 0x00409C99 (+11 more)  
  Called across various instruction handlers to fetch, read, or evaluate a variable or operand from the state/VM structure (in EAX), storing the retrieved value into a local buffer pointer passed in ECX.

### VM_PushValue - `0x004098B0`

_original name `sub_4098B0` | `bool __fastcall VM_PushValue(VMContext *vm, const Value64 *valPtr)` | confidence: high | clear memory offsets, standard stack-push pattern with bounds checking | source: llm_

**Pushes a 64-bit value onto the virtual machine's internal stack, checking for overflow and setting an error flag if the stack limit (256 entries) is exceeded.**

The function checks the current stack pointer located at offset 0x998 of the VM context (passed in EAX). If the index has reached or exceeded 0x100 (256), it sets an overflow/error flag at offset 0xA68 of the VM context and returns 0 (false). Otherwise, it reads a 64-bit value from the pointer passed in EDX, stores it into the VM stack array at `0x198 + index * 8`, increments the stack pointer at offset 0x998 by 1, and returns 1 (true).

**Parameters:**
- `vm` (VMContext *, eax) - Pointer to the virtual machine state structure.
- `valPtr` (const void *, edx) - Pointer to the 64-bit value structure to be pushed.

**Returns:** bool (1 for success, 0 for stack overflow) returned in al

**Side effects:**
- Modifies the stack pointer integer at VMContext+0x998
- Writes a 64-bit value into the stack buffer at VMContext+0x198+index*8
- Sets an error flag at VMContext+0xA68 to 1 if a stack overflow occurs

**Data structures:**
- VMContext: +0x198 stack array (256 entries of 8 bytes each), +0x998 stack count/index (int), +0xA68 stack overflow/error flag (byte)

**Called by:**
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409A8B, 0x00409ADC, 0x00409AF9, 0x00409B37, 0x00409BB7, 0x00409C01, 0x00409C2E, 0x00409C89 (+9 more)  
  Called throughout opcode handling to push or store a computed value, result, or variable back into the VM/state buffer or register set, passing the 'this' pointer in EAX and a pointer to the value structure in EDX.

### InitializeStateBlock - `0x004097E0`

_original name `sub_4097E0` | `bool __thiscall InitializeStateBlock(void *this)` | confidence: high | standard object initialization pattern with constant table population and boundary validation | source: llm_

**Initializes a state or configuration block object via `this` (passed in `ecx`), setting up vtable/type pointers, zeroing fields, and populating internal configuration tables from constant data.**

The function operates on an object instance passed via `ecx`. It sets up initial pointer/type fields (such as `this+8` and `this+0Ch` with constant 0x2AB), zeroes out several fields at specific offsets (`+0`, `+4`, `+998h`, `+99Ch`, `+0A60h`, etc.), and configures array/count fields at `+90h` (value 6) and `+194h` (value 2). It then enters a loop running up to 6 iterations (`[ecx+90h]`), copying 4-byte chunks of data from a read-only constant table (`unk_41CF7C`) into an internal array (`this+10h`). During this loop, it checks if a loaded word equals 0x2AB; if it is not below (i.e. greater than or equal), it immediately aborts and returns `false`. If all iterations succeed, it enters a second loop based on `+194h` to initialize secondary table entries, and finally returns `true`.

**Parameters:**
- `this` (void *, ecx) - Pointer to the state or configuration block object being initialized.

**Returns:** bool in al (1/true on successful initialization, 0/false if validation fails during table population)

**Side effects:**
- Modifies fields and internal buffers of the object instance pointed to by ecx.

**Data structures:**
- StateBlock: +0x0 void*, +0x4 void*, +0x8 void*, +0x0Ch int, +0x10h char[24], +0x90h int, +0x94h void*, +0x194h int, +0x998h int, +0x99Ch int, +0x0A60h int, +0x0A64h int, +0x0A68h char

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401F2E  
  Called with a pointer to a validation/state block (`ebx+56768h`); its boolean return value dictates whether the function proceeds or exits early with failure (`0`).

### InitializeEntityRegistration - `0x00404570`

_original name `sub_404570` | `void __fastcall InitializeEntityRegistration(void *esi_entity_struct, void *edx_unused, int arg_id)` | confidence: high, clear structure manipulation patterns, well-typed callee signatures, and standard container management logic. | source: llm_

**Initializes a new entity/resource structure and registers it into a global tracking array or proximity-matching container.**

The function begins by zeroing out or initializing various fields within the newly allocated entity structure passed in esi, storing the provided identifier from eax. It initializes state tracking variables, sets invalid or default index/flag values, and copies configuration offsets from global state structures located via dword_42AC60. It then scans a global registration container/array (managed via pointers at edx+51EB0h) to check if the current entity is already registered with the proximity-matching callback (sub_404640). If not found, it ensures the container has sufficient capacity using sub_4198B0 (ResizeContainer) if the current count matches capacity, and appends the new entity-callback pair into the dynamic list, incrementing the entry count.

**Parameters:**
- `entity_struct` (void*, esi) - Pointer to the entity structure being initialized and registered.
- `identifier` (int, eax) - An identifier or index value assigned to the entity.

**Returns:** none

**Side effects:**
- Initializes fields of the entity structure at esi
- May resize the global tracking container via sub_4198B0
- Registers the entity and callback sub_404640 into the global registration list

**Data structures:**
- EntityStruct: +0x0 id, +0x4/+0x8/+0xCh/+14h status fields, +0x18 flags (-1), +0x34 config value, +0x38 config value, +0x48 buffer/array fields
- ContainerStruct: +0x0 data pointer, +0x4 current count, +0x8 capacity

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401F12  
  Called in a loop iterating `ebx+5BBBCh` times, passing an offset into a sub-array (`esi`) and the loop index (`edi`) to execute per-element operations.

**Calls:**
- `CheckProximityMatch` (0x00404640) - address taken at 0x004045E3, 0x004045F0
- `ResizeContainer` (0x004198B0) - call at 0x00404618

### ResizeContainer - `0x004198B0`

_original name `sub_4198B0` | `void* __fastcall ResizeContainer(int capacity_hint, void *container_struct)` | confidence: high | matched container growth and reallocation pattern precisely based on disassembly and caller context | source: llm_

**Resizes the underlying data array of a dynamic container structure, doubling or scaling capacity until it can satisfy a required minimum size hint.**

The function calculates a new capacity for a dynamic container. It inspects the current capacity stored at `container_struct+8`. If it is zero, it initializes a default capacity of 16 (0x10); otherwise, it doubles the current capacity (`eax * 2`). It then enters a loop that continues doubling `ebx` until it is greater than or equal to the requested minimum capacity passed in `ecx`. Once the target capacity is determined, it calculates the new byte size (multiplying capacity by 8, implying elements are 8 bytes each) and calls ReallocateCustomBlock to resize the underlying data buffer at `container_struct+0`. Finally, it updates the container structure with the new buffer pointer and the new capacity value, and returns the new buffer pointer.

**Parameters:**
- `capacity_hint` (size_t, ecx) - Minimum required capacity or size hint for the expansion.
- `container_struct` (void*, [esp+8]) - Pointer to the container tracking structure.

**Returns:** void* in eax, pointing to the reallocated buffer

**Side effects:**
- Reallocates the memory buffer stored inside the container structure
- Updates the buffer pointer and capacity fields within the container structure

**Data structures:**
- ContainerStruct: +0x0 void* data_ptr, +0x8 size_t capacity

**Called by:**
- `InitializeEntityRegistration` (0x00404570) - call at 0x00404618  
  Called with a pointer to the table control structure (`edi`) and a capacity/size hint (`ecx+1`) when the current tracking array has reached its maximum capacity (`[edi+8]`), presumably to resize, reallocate, or expand the underlying collection before a new element is added.

**Calls:**
- `ReallocateCustomBlock` (0x00418670) - call at 0x004198E0

### CheckProximityMatch - `0x00404640`

_original name `sub_404640` | `void __cdecl CheckProximityMatch(const void *arg0, void *arg1)` | confidence: high, clear logic flow involving pointer dereferences, distance calculation, and state updates matching caller expectations. | source: llm_

**Validates entity state and computes spatial distance against a threshold to update registration or trigger status flags.**

The function inspects a structure passed as `arg_0`, extracting nested pointers (`[eax+4]` and `[eax+8]`). It validates that these pointers are non-null and checks state flags within the referenced object (such as a byte at offset 0x1B matching an integer at `[esi]` and specific bit patterns at `[eax+1C]`). If these checks pass, it extracts coordinates, computes the Euclidean distance between a point embedded in the object and a point derived from `arg_1` (`esi`) using `CalculateDistance`, and compares the result against a float constant. If the distance condition is met, it updates state fields in `arg_1` (setting a flag at offset 0x1C and copying coordinate data from `edi`).

**Parameters:**
- `context_or_params` (const void *, [esp+4]) - Pointer to a container or configuration structure holding nested entity/pointer references.
- `target_struct` (void *, [esp+8]) - Pointer to a structure (referenced as esi) containing coordinates, threshold data, and state flags to be updated.

**Returns:** none

**Side effects:**
- modifies fields within the target structure pointed to by arg_1 (sets a byte at offset 0x1C and writes coordinates at offsets 0x20 and 0x24)

**Data structures:**
- Target structure (esi): +0x00 threshold/ID (int), +0x1C status byte, +0x20/0x24 coordinate data, +0x34 point data
- Nested structure (referenced via arg_0 + 4): +0x1B byte flag, +0x1C status flags
- Nested structure (referenced via arg_0 + 8): +0x08 and +0x0C coordinate or object pointers

**Called by:**
- `InitializeEntityRegistration` (0x00404570) - address taken at 0x004045E3, 0x004045F0  
  The address of sub_404640 is referenced as a function pointer or callback identifier (vtable-like slot) and stored alongside the current structure pointer (esi) into a global tracking array, and is also used in a search loop to check if the structure is already registered.

**Calls:**
- `CalculateDistance` (0x00417F80) - call at 0x00404679

### CalculateDistance - `0x00417F80`

_original name `sub_417F80` | `double __cdecl CalculateDistance(float x1, float y1, float x2, float y2)` | confidence: high, standard Euclidean distance formula implementation using x87 FPU and _CIsqrt. | source: llm_

**Computes the Euclidean distance between two 2D points (x1, y1) and (x2, y2).**

The function calculates the Euclidean distance between two points by computing the square root of the sum of squared coordinate differences. It loads the coordinate values from the stack (or registers depending on calling convention), computes $(x_2 - x_1)^2 + (y_2 - y_1)^2$ using x87 FPU instructions, calls the square root routine `_CIsqrt`, and returns the final floating-point distance in st(0).

**Parameters:**
- `x1` (float, [esp+4]) - X coordinate of the first point
- `y1` (float, [esp+8]) - Y coordinate of the first point
- `x2` (float, [esp+12]) - X coordinate of the second point
- `y2` (float, [esp+16]) - Y coordinate of the second point

**Returns:** st(0) (floating-point distance value)

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x00406B00  
  (The caller's analysis gave no description of this use. Plain facts: call at 0x00406B00.)
- `FindNearestValidEntityOrTarget` (0x00406540) - call at 0x004065FF  
  Called inside the entity iteration loop at address 0x004065FF when an entity passes initial filtering. Passes an entity/substructure pointer in ecx and an offset from arg_0 in eax, returning a floating-point value (in st(0)) used to evaluate a distance or proximity threshold.
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x00407372, 0x0040747E, 0x004077B8, 0x004077EF  
  Called in loops iterating over candidate entities to compute distance, position, or metric values relative to a reference position or target.
- `CheckProximityMatch` (0x00404640) - call at 0x00404679  
  Called as part of a validation check on the control block, passing an adjusted pointer (esi + 34h) and an offset into the object (ecx + 8) as arguments, with its floating-point return value subsequently tested to determine whether to proceed with updating the structure state.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00417FB1

### EntityInteractionEventCallback - `0x00402980`

_original name `sub_402980` | `__thiscall void EntityInteractionEventCallback(void *this, int arg1, int arg2)` | confidence: high | clean and straightforward callback function structure matching the caller's usage as a function pointer. | source: llm_

**Serves as a callback wrapper that validates an object context and state before forwarding an event to the entity interaction handler.**

The function expects a pointer structure (commonly the 'this' or context pointer) on the stack. It checks if the pointer at offset +4 is non-null and whether the byte at offset +0x1B within that sub-structure equals 4. If these validation checks pass, it extracts a value from offset +8 into edi, computes an adjusted value from the second argument (adding the constant 0x571D4), and delegates the interaction handling to sub_4084C0 [HandleEntityInteractionEvent].

**Parameters:**
- `context` (void*, [esp+arg_0]) - Context structure containing internal pointers at +4 and +8.
- `event_arg` (int, [esp+arg_4]) - Event-related value or offset used to compute the parameter for the handler.

**Returns:** none

**Side effects:**
- Conditionally invokes HandleEntityInteractionEvent if validation checks succeed.

**Data structures:**
- Context: +0x4 sub_structure_ptr, +0x8 entity_index_or_handle
- SubStructure: +0x1B state_byte

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - address taken at 0x00401DC0, 0x00401EE9  
  Address taken and stored as a function pointer into a callback array (`[eax+ecx*8]`) within the resource structure to register an event handler or callback routine.

**Calls:**
- `HandleEntityInteractionEvent` (0x004084C0) - call at 0x0040299F

### HandleEntityInteractionEvent - `0x004084C0`

_original name `sub_4084C0` | `void __fastcall HandleEntityInteractionEvent(void *arg1 /* in eax */, void *arg2 /* in ecx */, void *arg3 /* in edi */)` | confidence: high, clear register usage, bounds checks, and control flow mapping directly to known callees. | source: llm_

**Validates an entity or resource index from an object context, optionally resolves a handle, and conditionally invokes an entity interaction processor.**

The function begins by inspecting a field at offset +0x238 of the object pointer passed in eax. If this value is negative or greater than or equal to a limit stored at offset +0x1B0 of the structure in ecx, or if the pointer in edi is null, the function immediately returns. Otherwise, it scales the index by 36 (using lea edx, [eax+eax*8] and multiplying by 4) to access an array element at `ecx + edx*4`. It checks a byte flag at offset +0x0B of this element; if zero, it prepares `esi` (the element pointer) and `edi` as arguments and performs a tail jump to `ProcessEntityInteractionContext`. If the flag is non-zero, it extracts a 32-bit value from offset +0x20 of the array element and calls `LookupResourceOrHandle`. If the resulting handle resolves to a valid non-zero pointer, it performs a floating-point load and store operation (likely referencing a constant value for floating-point validation or conversion) and returns; otherwise, it falls through to the same tail jump to `ProcessEntityInteractionContext` using the element pointer and `edi`.

**Parameters:**
- `context_or_object` (void*, eax) - Primary object pointer containing an index at offset +0x238.
- `base_structure` (void*, ecx) - Base structure pointer containing bounds at +0x1B0 and an array starting around +0x0B.
- `entity_context` (void*, edi) - Entity interaction context pointer passed along to the tail-called function.

**Returns:** none

**Side effects:**
- May perform a resource/handle lookup via sub_4029F0
- Tail-calls sub_4080A0 [ProcessEntityInteractionContext] under specific conditional branches

**Data structures:**
- Primary object: +0x238 int index
- Base structure: +0x1B0 int limit, +0x0B byte flag, array elements of size 36 bytes starting at array base with +0x20 handle/id field

**Called by:**
- `EntityInteractionEventCallback` (0x00402980) - call at 0x0040299F  
  Invoked as a conditional handler or processing action inside the callback function. It is called only when the object pointer extracted from the resource structure (at offset +4) is non-null and has a specific type/status byte at offset +1Bh equal to 4. It receives an adjusted value from the second argument (arg_4 plus an offset of 0x571D4) in ecx, while its other arguments are implicitly set up via registers like edi (derived from offset +8 of the first argument).

**Calls:**
- `LookupResourceOrHandle` (0x004029F0) - call at 0x004084E7
- `ProcessEntityInteractionContext` (0x004080A0) - tail jump at 0x00408500

### ProcessEntityInteractionContext - `0x004080A0`

_original name `sub_4080A0` | `void __fastcall ProcessEntityInteractionContext(void *record, void *context)` | confidence: high | well-structured loop iterating over 4 entity handles with clear calls to validation and command processing functions | source: llm_

**Iterates through a list of entity references in a context structure, validates each entity against compatibility and state rules, and triggers an entity interaction or command using helper functions.**

The function sets up a loop that runs 4 times, checking an array of entity references stored at offset +0x0C within the context/parameter structure passed in `eax`. For each iteration, it extracts a handle/ID from `edi`, looks up the corresponding entity pointer from a global entity table managed via `dword_42AC60`, and performs a series of validation checks: verifies entity flags, checks a linked list state (traversing via offset +0x1C and setting a flag at +0x0E), ensures the entity has no active blocking/flag at +1F4h, and calls `ValidateEntityPairCompatibility` between the entity (`ebx`) and the record (`esi`). If all checks pass, it populates a temporary structure on the stack with coordinate/state data from `esi` and invokes `ProcessEntityInteractionOrCommand` on the entity.

**Parameters:**
- `record` (void*, eax) - Record or source entity pointer holding flags at +0Bh, +20h, etc.
- `context` (void*, ecx) - Context pointer containing entity references at offset +0Ch and other data fields.

**Returns:** none

**Side effects:**
- Modifies fields within the record structure passed in eax (such as +0Bh and +20h)
- Updates state flags within matched entities and processes interactions via sub_40DAC0

**Data structures:**
- Global entity table referenced via dword_42AC60 holding entity pools and counts
- Context/Parameter structure with an array of entity handles starting at +0Ch

**Called by:**
- `HandleEntityInteractionEvent` (0x004084C0) - tail jump at 0x00408500  
  Jumped to via a tail call (jmp) when either the flag check at [ecx+edx*4+0Bh] is zero or the preceding call to sub_4029F0 returns a non-zero value. It receives the record pointer esi in eax and the context pointer edi in ecx to perform downstream processing.

**Calls:**
- `ValidateEntityPairCompatibility` (0x0040CAF0) - call at 0x0040815B
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x00408192

### ProcessEntityInteractionOrCommand - `0x0040DAC0`

_original name `sub_40DAC0` | `bool __fastcall ProcessEntityInteractionOrCommand(void *entity, void *paramStruct, int arg0)` | confidence: high, all callee relationships and structure accesses align cleanly with caller context and disassembler annotations. | source: llm_

**Validates, resolves, and processes an entity interaction or command based on state flags and type constraints, optionally queueing or executing state transitions.**

The function begins by inspecting state flags at offset +0x18 of the entity object (`ebx`). If specific bit conditions are not met, it invokes a virtual method at offset +0x20 of the entity's vtable. It then extracts interaction parameters from the passed structure (`esi`), optionally resolving the interaction type via `ResolveEntityInteractionType` if the entity type code is 7. It performs validation on the resulting entity type using `IsValidEntityType` and checks floating-point conditions. Depending on the resolved interaction/entity type (such as types 8, 9, or 10), it searches for associated resource entries using `FindResourceEntryByParam`. Depending on `arg0`, it either records the interaction data into a ring buffer/array at offset +0x100 within the entity structure or performs full state update actions, including cleaning up secondary entities via `CleanupSecondaryEntityOrResource`, processing entity sub-items via `ProcessEntitySubItems`, resetting state fields via `ResetEntityStateFields`, and updating internal state and property slots.

**Parameters:**
- `entity` (void*, ecx) - Pointer to the primary entity object being processed.
- `paramStruct` (void*, eax) - Pointer to a parameter structure containing interaction/descriptor fields.
- `arg0` (int, esp+4) - Flag or mode specifier controlling whether to append to the interaction queue or execute immediate state transition logic.

**Returns:** bool (1 on success/completion, 0 on validation failure)

**Side effects:**
- Modifies entity state fields, flags, and internal buffers (including offsets +0x70, +0x100, +0x1F4, +0x204, etc.)
- Calls virtual methods on the entity and its associated resource entries
- Cleans up secondary resources and processes sub-items when performing state transitions

**Data structures:**
- Entity: +0x18 flags, +0x5D state flag, +0x70-0x7C state fields, +0x100 queue count, +0x108+ buffer array, +0x1F4 computed field, +0x204/+0x208 cached values

**Called by:**
- `ResetOrClearEntityReferences` (0x004081B0) - call at 0x00408261  
  Invoked inside the loop for a valid referenced entity (passed via `ecx = esi`) after checking its status flags. It is passed a pointer on the stack (`var_14` initialized to structure containing {3, 4, 0, 0} and fields from `esi+230h` / `esi+234h`) as an argument. The call acts on the entity object, presumably triggering a primary action, activation, or event notification routine.
- `RtsManager::ProcessCommandOrActionBatch` (0x00408D70) - call at 0x00408F15  
  Called during operation type 0 (unconditional execution path or after argument preparation), passing a pointer to a prepared local argument structure (`&var_14`) and an operation flag derived from `ebp+3` to execute the core command handler, incrementing a success counter upon completion.
- `ExecuteEntityInteractionOrCommandOpcode` (0x0040AA60) - call at 0x0040AB60  
  Called after setting up a local parameter block on the stack (including floating-point conversions and retrieved attributes) to execute a core computation or subsystem routine, with its status/return value used to populate the output status structure.
- `ProcessEntityInteractionContext` (0x004080A0) - call at 0x00408192  
  Called as the primary action step within the loop when all prior validity checks and `sub_40CAF0` succeed. It is invoked with the object pointer (`ebx`) passed in `ecx`, a pointer to a constructed local parameters/descriptor struct on the stack in `eax`, and `0` pushed on the stack.

**Calls:**
- `ResolveEntityInteractionType` (0x0040D900) - call at 0x0040DB15
- `IsValidEntityType` (0x0040DA90) - call at 0x0040DB4C
- `FindResourceEntryByParam` (0x0040BED0) - call at 0x0040DB61, 0x0040DB84, 0x0040DBF0, 0x0040DC02
- `CleanupSecondaryEntityOrResource` (0x00413930) - call at 0x0040DC13
- `ProcessEntitySubItems` (0x0040BF70) - call at 0x0040DC18
- `ResetEntityStateFields` (0x0040C100) - call at 0x0040DC27

### ResetEntityStateFields - `0x0040C100`

_original name `sub_40C100` | `void __fastcall ResetEntityStateFields(void *entity)` | confidence: high, straightforward sequence of field zeroing instructions with a clear pointer parameter in eax. | source: llm_

**Resets several internal state and property fields within an entity structure to zero.**

The function zeroes out multiple specific member fields of an entity object passed via the eax register. It uses a single zeroed register (ecx) to clear a byte at offset +0x1F0, a 32-bit dword at offset +0x1C4, another 32-bit dword at offset +0x1C8, a byte at offset +0x1D4, and another 32-bit dword at offset +0x1EC. There are no branches or loops, and the function returns void.

**Parameters:**
- `entity` (void*, eax) - Pointer to the entity structure whose fields are being reset.

**Returns:** none

**Side effects:**
- Modifies fields at offsets +0x1C4, +0x1C8, +0x1D4, +0x1EC, and +0x1F0 of the entity structure pointed to by eax by setting them to zero.

**Data structures:**
- Entity object: +0x1C4 dword field, +0x1C8 dword field, +0x1D4 byte field, +0x1EC dword field, +0x1F0 byte field

**Called by:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DC27  
  Called with `eax = ebx` to refresh, re-initialize, or notify the main entity object after modifying its internal state parameters.

### ProcessEntitySubItems - `0x0040BF70`

_original name `sub_40BF70` | `void __fastcall ProcessEntitySubItems(void *ebx)` | confidence: high | disassembly is straightforward and cleanly maps to container iteration and virtual method dispatch. | source: llm_

**Iterates over an array of sub-objects/items contained within an entity if a specific flag is set, invoking a virtual method on each item that meets criteria.**

The function first checks a bit flag (`0x100`) at offset `+0x18` of the entity structure pointed to by `ebx`. If the flag is not set, it immediately returns. If set, it enters a loop running `byte ptr [ebx+5Eh]` times. For each iteration, it inspects an array of pointers starting at offset `+0x210` of the entity. If a byte at offset `+0x13` of the sub-object is non-zero, it invokes a virtual function located at offset `+0x1C` of the sub-object's virtual method table. Finally, it clears the `0x100` flag from offset `+0x18` before returning.

**Parameters:**
- `entity` (void*, ebx) - Pointer to the main entity structure.

**Returns:** none

**Side effects:**
- Invokes virtual methods on sub-objects stored within the entity structure
- Clears bit 0x100 at offset +0x18 of the entity structure

**Data structures:**
- Entity: +0x18 WORD flags, +0x5E BYTE sub_item_count, +0x210 array of sub-item pointers
- SubItem: +0x13 BYTE status_flag, +0x0 vtable pointer
- VTable: +0x1C function pointer

**Called by:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DC18  
  Called as part of the state reset or update sequence following type-specific validation and cleanup operations.

### CleanupSecondaryEntityOrResource - `0x00413930`

_original name `sub_413930` | `void __fastcall CleanupSecondaryEntityOrResource(void *esi)` | confidence: high | clear control flow and field interactions matched with caller context | source: llm_

**Cleans up a secondary entity or referenced resource associated with an object, decrementing a reference count if conditions are met and resetting state flags.**

The function checks several conditions on the object passed in `esi`: if the handle/pointer at `esi+24h` is zero, or if the byte at `esi+1Ch` is zero, 4, or 5, it bypasses resource lookup and proceeds directly to resetting the state byte at `esi+1Ch` to zero. Otherwise, it calls LookupResourceOrHandle on the value at `esi+24h` to retrieve a referenced object. If that object is valid and its internal flags (at `eax+1Ch`, tested with 0x80) indicate active status, it checks the integer counter at `eax+2C4h`. If this counter is greater than zero, it decrements it by one. Finally, it clears the state byte at `esi+1Ch` to zero and returns.

**Parameters:**
- `entity` (void*, esi) - Pointer to the primary entity/object being cleaned up

**Returns:** none

**Side effects:**
- modifies the byte at [esi+1Ch]
- decrements a reference/count field at [resource+2C4h] if positive

**Data structures:**
- Entity: +0x1C state byte, +0x24h resource handle/id
- ManagedResource: +0x1C flags pointer/word, +0x2C4h reference count or counter

**Called by:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DC13  
  Called on a secondary entity or sub-object returned by a prior check (`esi = eax`) to perform cleanup, destruction, or state finalization.

**Calls:**
- `LookupResourceOrHandle` (0x004029F0) - call at 0x00413949

### FindResourceEntryByParam - `0x0040BED0`

_original name `sub_40BED0` | `void* __fastcall FindResourceEntryByParam(void *esi, int edi)` | confidence: high | clean, straightforward pointer array search loop with clear field references | source: llm_

**Searches an array of pointers within a resource object at offset 0x210 to find an entry matching a specific property value (`edi`), returning the matching pointer or NULL.**

The function reads a count from `byte ptr [esi+5Eh]`. If this count is less than or equal to zero, it immediately returns NULL (0). Otherwise, it loops through an array of pointers located at `esi + 0x210` up to `ecx` elements. For each element, it dereferences the pointer, reads a byte from offset 0x10 of the target structure (`[ebx+10h]`), and compares it against `edi`. If a match is found, it returns the pointer from the array (`[esi+eax*4+210h]`). If the loop finishes without finding a match, it returns NULL.

**Parameters:**
- `resourceObj` (void*, esi) - Pointer to the main resource or entity object containing the array and count.
- `targetValue` (int, edi) - Value to match against the byte field at offset 0x10 of each referenced sub-structure.

**Returns:** Pointer to the matching sub-structure from the array, or NULL if no match is found, returned in eax.

**Data structures:**
- ResourceObject: +0x5E byte count, +0x210 pointer array
- SubStructure: +0x10 byte property used for matching

**Called by:**
- `RtsManager::ProcessCommandOrActionBatch` (0x00408D70) - call at 0x00408E54  
  Called inside the main loop for a specific operation type (when byte [ebp+0] is 2) passing `esi` (the validated object pointer) and `edi` (derived from a word at [ebp+36h]) to look up or acquire a sub-component/state, checking its flags before updating an activation state and incrementing a success counter.
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DB61, 0x0040DB84, 0x0040DBF0, 0x0040DC02  
  Invoked at multiple points with `esi = ebx` and `edi` set to 0 or 1 to check internal entity/resource conditions or states, aborting or branching based on whether the returned eax is zero or non-zero.

### IsValidEntityType - `0x0040DA90`

_original name `sub_40DA90` | `bool __fastcall IsValidEntityType(unsigned int entityType)` | confidence: high, clean switch/jump-table implementation with straightforward range checking and boolean return paths | source: llm_

**Validates whether a given entity type code falls within an accepted set using a switch/jump table mechanism.**

The function takes an entity type code (passed via eax) and adjusts it by subtracting 3 (`add eax, 0FFFFFFFDh`), effectively mapping type code 3 to index 0. It then compares the adjusted value against 0Bh (11) using an unsigned comparison (`ja`), routing out-of-range values to a default handler block. Values within range are used as an index into a lookup table (`byte_40DAB4`) to obtain an offset or secondary selector, which in turn indexes a jump table (`jpt_40DA9F`). Ultimately, the control flow resolves to either returning true (`al = 1`) for valid entity types or false (`al = 0`) for invalid ones.

**Parameters:**
- `entityType` (unsigned char, eax)

**Returns:** Returns a boolean value in al (non-zero/1 for valid, 0 for invalid).

**Called by:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DB4C  
  Called to validate the entity type code (passed as zero-extended `cl`) against specific criteria; if it returns non-zero, the function aborts and returns false.

### ResolveEntityInteractionType - `0x0040D900`

_original name `sub_40D900` | `void* __fastcall ResolveEntityInteractionType(void *entityA, void *edi_buffer, void *entityB)` | confidence: high, clear control flow paths mapping out entity relationship rules and interaction state classification. | source: llm_

**Determines the interaction type code between two entities and populates a result buffer starting with the computed code.**

The function copies a 16-byte header from the primary entity (passed in eax) into the destination buffer (edi), then attempts to resolve a secondary entity reference using global manager tables if certain flags are set. Depending on the resolved secondary entity's attributes (type codes, state flags, faction/team bytes) and the caller-provided entity (ebx/arg_0), it evaluates various rule sets—including calling ValidateEntityPairCompatibility—to classify the interaction, writing an integer interaction/status code (e.g., 3, 4, 8, 9, or a dynamically computed code) into the first byte of the destination buffer before returning it in eax.

**Parameters:**
- `entityA` (void*, eax) - Primary entity pointer whose attributes are initially copied.
- `buffer` (void*, edi) - Destination buffer where the output state/interaction structure is written.
- `entityB` (void*, stack arg_0 ([esp+8])) - Secondary entity pointer evaluated during compatibility and rule checks.

**Returns:** Returns the destination buffer pointer (edi) in eax.

**Side effects:**
- Writes up to 16 bytes (or more based on interaction type) to the destination buffer pointed to by edi.

**Data structures:**
- Entity: +0x00 dword ID/data, +0x04 dword flags/type, +0x08 dword, +0x0C dword, +0x1B byte faction/type, +0x1C dword flags/pointer

**Called by:**
- `ProcessEntityInteractionOrCommand` (0x0040DAC0) - call at 0x0040DB15  
  Called to retrieve or transform attribute/type data into a local structure when the primary entity type is 7, passing the entity pointer in eax and a destination buffer pointer in edi.

**Calls:**
- `ValidateEntityPairCompatibility` (0x0040CAF0) - call at 0x0040D9CE, 0x0040DA59

### ValidateEntityPairCompatibility - `0x0040CAF0`

_original name `sub_40CAF0` | `bool __fastcall ValidateEntityPairCompatibility(void *entityA, void *entityB)` | confidence: high, clear register-based conventions and sequential validation gates matching caller usage context. | source: llm_

**Performs a multi-stage validation check between two entities (passed in ebx and esi) to determine if they are compatible for interaction or spatial association.**

The function performs a series of validation gates using fields from two entity structures, referenced via registers ebx and esi. First, it extracts a pointer from [ebx+1Ch] (edi) and checks if [edi+0Ch] is non-zero. It ensures esi is non-zero, that bit 0 of [esi+18h] is not set (by checking `not al` & 1), and that esi is not equal to ebx. It also verifies that bit 4 of [ebx+18h] is not set. Next, it checks the byte at [esi+1Ah]. If this byte equals 3, it validates that [edi+0Bh] equals 2. If the byte at [esi+1Ah] equals 1, it calls sub_40BD40 [ValidateEntitySubComponent] on esi, checks that bit 6 of [esi+18h] is not set, and calls sub_40BCD0 with esi and ebx to verify compatibility. Finally, it checks bit 3 of the byte at the pointer stored in [esi+1Ch], which determines whether to test bit 10 or bit 11 of the 16-bit value at [edi], returning the resulting bit as a boolean.

**Parameters:**
- `entityA` (void*, ebx) - Primary entity pointer
- `entityB` (void*, esi) - Secondary entity or sub-component pointer

**Returns:** bool in al (1 if compatible, 0 otherwise)

**Data structures:**
- `Entity`

**Called by:**
- `ValidateAdvancedEntityMatch` (0x0040CAA0) - call at 0x0040CADC  
  Called at the end of the function after passing all validation gates (including flag checks on `esi+18h` and a zero check on `esi+60h`), passing data derived from `edi` (`[edi]`) to perform the primary processing or action for this component/entity pair.
- `ProcessEntityInteractionContext` (0x004080A0) - call at 0x0040815B  
  Called inside the main loop iteration for a validated handle/object (ebx) when `esi` is non-zero and `[ebx+1F4h]` is zero. Its boolean return value in `al` is checked; if non-zero, it proceeds to invoke `sub_40DAC0`.
- `ResolveEntityInteractionType` (0x0040D900) - call at 0x0040D9CE, 0x0040DA59  
  Called as a boolean validation or condition check helper during specific entity interaction or state checks (at 0x0040D9CE and 0x0040DA59), receiving context/pointers via registers/stack (such as ebp/ebx), and its boolean/status return value (in al) is used to determine whether to branch to a success state (setting result code 4) or fallback/alternative failure paths.

**Calls:**
- `ValidateEntitySubComponent` (0x0040BD40) - call at 0x0040CB29
- `sub_40BCD0` (0x0040BCD0) - call at 0x0040CB3C

### sub_40BCD0 - `0x0040BCD0`

_source: llm (unparsed response)_

**{ "suggested_name": "ValidateEntityCompatibility", "prototype": "bool __fastcall ValidateEntityCompatibility(void *contextObject, void *entityReference)", "short_summary": "Performs a secondary compatibility and state validation check between a context object (passed in eax) and an entity reference (passed in edx) using a lookup table.", "detailed_summary": "The function extracts a byte value from offset 0x1B of both the context object (eax) and the entity reference (edx), multiplies them together as signed/unsigned values, and adds the result to a base address from global data (`dword [...]**

{
  "suggested_name": "ValidateEntityCompatibility",
  "prototype": "bool __fastcall ValidateEntityCompatibility(void *contextObject, void *entityReference)",
  "short_summary": "Performs a secondary compatibility and state validation check between a context object (passed in eax) and an entity reference (passed in edx) using a lookup table.",
  "detailed_summary": "The function extracts a byte value from offset 0x1B of both the context object (eax) and the entity reference (edx), multiplies them together as signed/unsigned values, and adds the result to a base address from global data (`dword_42AC60`). It fetches a byte from this computed table entry to perform initial boundary and flag checks. If the configuration matches specific exclusion criteria (e.g., if ecx equals 5 or bit 0x10 is set in the table entry+3), it returns false (0). Otherwise, it constructs a bitmask using `1 << ecx` and checks it against a field at offset 0xE8 of the table entry structure, returning true if the bit is clear and false otherwise.",
  "parameters": [
    {
      "name": "contextObject",
      "location": "eax",
      "type": "void*",
      "description": "Context object containing an identifier byte at offset +0x1B"
    },
    {
      "name": "entityReference",
      "location": "edx",
      "type": "void*",
      "description": "Entity reference containing an identifier byte at offset +0x1B"
    }
  ],
  "return_value": "bool in al (1 if compatible/valid, 0 otherwise)",
  "side_effects": [],
  "data_structures": [
    {
      "name": "ContextObject",
      "offsets": [
        {
          "offset": "+0x1B",
          "type": "unsigned char",
          "description": "Entity type or category index"
        }
      ]
    },
    {
      "name": "EntityReference",
      -offsets: [
        {
          "offset": "+0x1B",
          "type": "unsigned char",
          "description": "Entity type or category index"
        }
      ]
    },
    {
      "name": "CompatibilityTableEntry",
      "offsets": [
        {
          "offset": "+0x0",
          "type": "unsigned char",
          "description": "Comparison target byte"
        },
        {
          "offset": "+0x3",
          "type": "unsigned char",
          "description": "Flags byte (tested for bit 0x10)"
        },
        {
          "offset": "+0xE8",
          "type": "unsigned int",
          "description": "Bitmask used for final compatibility test"
        }
      ]
    }
  ],
  "confidence": "high | the register usage, table lookup mechanism, and bit-test logic are clear from the disassembly"
}

**Called by:**
- `ValidateEntityPairCompatibility` (0x0040CAF0) - call at 0x0040CB3C  
  Called after sub_40BD40 succeeds and additional bitflag checks on esi pass, passing esi in eax and ebx in edx to perform a secondary interaction or relationship validation between the two entities. Its boolean return value in al is tested to determine whether validation continues to final bitmask extraction.

### ValidateEntitySubComponent - `0x0040BD40`

_original name `sub_40BD40` | `bool __fastcall ValidateEntitySubComponent(void *object)` | confidence: high, straightforward bitwise checks and comparisons on a single structure pointer with clear boolean outputs | source: llm_

**Validates specific bit flags and a byte state field within an object structure to determine sub-component compatibility.**

The function checks bit fields located at offset +0x18 and a byte field at offset +0x60 of the passed object pointer (in eax). First, it inspects bit 0 of the inverted byte at [eax+18h]; if this bit is zero (meaning bit 0 of the original byte was set), it immediately fails and returns false. Next, it checks if any of the bits corresponding to the mask 0x206 are set in the word at [eax+18h]; if any are set, it returns false. Finally, it checks whether the byte at [eax+60h] is equal to zero, returning true if it is zero and false otherwise.

**Parameters:**
- `object` (void *, eax) - Pointer to the object or sub-state structure being validated.

**Returns:** bool in al (1 if validation passes, 0 if it fails)

**Data structures:**
- Object: +0x18 word/byte bitfield flags, +0x60 byte state indicator

**Called by:**
- `ValidateEntityPairCompatibility` (0x0040CAF0) - call at 0x0040CB29  
  Called when esi has a type/attribute sub-state of 1 ([esi+1Ah] == 1), passing the object pointer in eax (esi) to perform a preliminary sub-component validity check. Its boolean return value in al is tested, causing the parent function to fail validation if zero.

### LookupResourceOrHandle - `0x004029F0`

_original name `sub_4029F0` | `void* __fastcall LookupResourceOrHandle(unsigned int handle)` | confidence: high, clean register-based handle translation logic matching typical resource manager patterns. | source: llm_

**Resolves a 32-bit handle (comprising an index and a validation tag) into a managed object pointer, verifying bounds, a tag match, and an internal status byte.**

The function takes a 32-bit handle in `ecx`. It first checks if the handle is non-zero, returning 0 if null. It extracts the lower 16 bits as an index (minus 1) and compares it against a global maximum count stored at `[data_42AC60 + 0x51E84]`. If out of bounds, it returns 0. Otherwise, it retrieves a table base pointer from `[data_42AC60 + 0x51E80]` and indexes into an array of 8-byte entries (`base + index * 8`). It then extracts the upper 16 bits of the original handle (`ecx >> 16`) and compares it with a 16-bit validation tag stored at `[entry + 4]`. If the tags do not match, it returns 0. Finally, it dereferences a pointer at `[entry + 0]`, checks if it is non-zero, and verifies that the byte at offset `+0x1A` of the resolved object equals 1. If all conditions are met, it returns the object pointer in `eax`; otherwise, it returns 0.

**Parameters:**
- `handle` (unsigned int, ecx) - A 32-bit identifier where the lower 16 bits represent a 1-based index and the upper 16 bits represent a generation or validation tag.

**Returns:** Returns a pointer to the resolved object in `eax`, or `0` on failure.

**Data structures:**
- Table Entry: +0x0 void* object_ptr, +0x4 unsigned short tag

**Called by:**
- `HandleEntityInteractionEvent` (0x004084C0) - call at 0x004084E7  
  Called conditionally when the indexed element's flag at [ecx+edx*4+0Bh] is non-zero. It receives a context pointer from [esi+20h] (where esi is the computed base of the indexed record) in ecx, and its boolean return value in eax determines whether the function continues to a tail call or exits early.
- `CleanupSecondaryEntityOrResource` (0x00413930) - call at 0x00413949  
  Called after verifying that the main entity `esi` has a valid non-zero pointer at offset `+24h` and its type byte at `+1Ch` is none of {0, 4, 5}. The return value (an object pointer in `eax`) is checked for null, and if valid, its internal fields at `+1Ch` and `+2C4h` are inspected to potentially decrement the integer counter at `+2C4h`.

### ProcessRtsResourceSpawningOrPathNodes - `0x00407B50`

_original name `sub_407B50` | `void __fastcall ProcessRtsResourceSpawningOrPathNodes(void *resourceBlock)` | confidence: high | clear control flow, recognizable entity creation and math loops matching standard game engine initialization patterns | source: llm_

**Iterates through waypoint or resource spawn definitions, evaluates spatial metrics and procedural selection checks via geometric calculations and pseudo-random generators, and instantiates new resource entities using sub_402560.**

The function validates that the resource structure's internal count (`arg_0 + 0x1B0`) is less than 0x0C. It then iterates over a collection of spatial points from a global manager table. For each entry, it calculates distances using _CIsqrt and checks collision or proximity metrics. It uses a pseudo-random number generator state (stored at `esi+51EF8h`) with bit-shifting and multiplication routines to determine sub-variant types, indexes, and probabilities. It calls sub_418050 to evaluate complex spatial/path metrics and calls sub_402560 to create and register resource entities when specific threshold conditions and spatial constraints are met, incrementing the active count.

**Parameters:**
- `resourceBlock` (void*, [ebp+8]) - Pointer to the main resource/state block structure being initialized or configured.

**Returns:** none

**Side effects:**
- Modifies fields within the resource block passed as argument, including count and array elements at +0x1B0
- Allocates and registers new resource entities via sub_402560

**Data structures:**
- ResourceBlock: +0x1B0 int count/index
- EntityInstance: +0x230 void*, +0x234 void*, +0x238 int

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401DA9  
  Called with a pointer to a status/control structure (`ebx+571D4h`), after zeroing fields within it, to perform validation or reset operations.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00407C23, 0x00407C8A
- `CalculateSpatialOrPathMetric` (0x00418050) - call at 0x00407D85
- `__ftol2_sse` (0x0041A160) - call at 0x00407E4F, 0x00407E96
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00407F3B

### SpawnResourceOrEntityBatch - `0x00402050`

_original name `sub_402050` | `void __stdcall SpawnResourceOrEntityBatch(void *resourceBlock, int resourceIndex)` | confidence: high based on clear function call patterns matching resource initialization and grid validation logic | source: llm_

**Processes a specific resource or state entry by calculating spatial metrics, normalizing vectors, validating grid positions, and spawning/registering entity instances via factory routines.**

The function performs per-item processing for resource blocks in a loop/batch framework. It begins by reading coordinate and configuration fields from the passed resource block (ebp) combined with an index offset (ebx). It normalizes 3D vectors using Vec3_NormalizeOrZero, computes spatial metrics using CalculateSpatialOrPathMetric, and clamps float values. It checks grid area validity and availability using CheckGridAreaPassesCriteria and CheckGridAreaOrFindNearby, iterating through radius loops and condition checks to find valid placement coordinates. When criteria are met, it invokes CreateAndRegisterResourceEntity to instantiate and register new resource/RTS entities into spatial tracking containers.

**Parameters:**
- `resourceBlock` (void*, [esp+70h+arg_0]) - Pointer to the large resource/state block ('RM' header structure).
- `resourceIndex` (int, [esp+70h+arg_4]) - Index or identifier of the resource item being processed.

**Returns:** none

**Side effects:**
- Allocates and registers new resource entities via CreateAndRegisterResourceEntity
- Modifies spatial grid cells and resource tracking containers

**Data structures:**
- `ResourceBlock`

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401C42  
  Called in a loop from `0` up to `ebx+5BBBCh` (resource count), passing the resource block pointer (`ebx`) and loop index (`edi`) to perform per-item processing.

**Calls:**
- `Vec3_NormalizeOrZero` (0x00417FD0) - call at 0x004020C9
- `CalculateSpatialOrPathMetric` (0x00418050) - call at 0x004020FB, 0x0040247D
- `ClampFloat` (0x00418140) - call at 0x00402106
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040211B, 0x004022ED, 0x00402537
- `__ftol2_sse` (0x0041A160) - call at 0x004021F0, 0x0040224C, 0x004023B9, 0x0040240D
- `CheckGridAreaPassesCriteria` (0x00414CE0) - call at 0x0040228B
- `CheckGridAreaOrFindNearby` (0x00414E40) - call at 0x0040245C

### CheckGridAreaPassesCriteria - `0x00414CE0`

_original name `sub_414CE0` | `bool __fastcall CheckGridAction(void *pDataStruct, int edx_val, int ecx_val, int arg0)` | confidence: high, clear bounds-checked 2D grid scanning algorithm with standard 0x60 (96) dimension limits and nested loop structures. | source: llm_

**Tests whether a 2D rectangular grid region within a structured data array meets specific validity and value criteria, returning true if all checked elements satisfy the conditions.**

The function performs a bounds-checked spatial region query over a grid of size 96x96 (0x60). It receives a base pointer via stack (arg_0), and coordinates/dimensions through registers eax, edx, and ecx. First, it extracts a reference value from a specific grid cell [edi + edi*2] * 32 + esi offset from the base structure at offset 0x9304. Then, it iterates through a 2D rectangular sub-region (defined by coordinate ranges), applying bounds checks [0, 0x60) for every accessed cell. For each cell within the loop, it validates flags or status bytes at offset 0x2400 from the row base against a bitmask (0x5B), and compares the cell's byte value against the initial reference value. If any check fails or mismatches, it immediately returns false (al = 0); otherwise, if all checked elements pass, it completes the loops and returns true (al = 1).

**Parameters:**
- `pDataStruct` (void*, stack arg_0 (at [esp+0Ch+arg_0])) - Pointer to the large data structure or resource block containing the grid arrays at offset 0x9304.
- `param_edx` (int, edx) - Coordinate or dimension parameter used for grid traversal.
- `param_ecx` (int, ecx) - Coordinate or dimension parameter used for grid traversal.
- `param_eax` (int, eax) - Coordinate or dimension parameter used for grid traversal.

**Returns:** bool in al (1 on success/match, 0 on failure/mismatch)

**Data structures:**
- Grid array: 2D grid structure of size 96x96 (0x60) located at offset +0x9304 in the main data structure, with sub-tables or attribute bytes at +0x2400 relative to row pointers.

**Called by:**
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x004058C1  
  Called inside nested loops over spatial or index grids (at 0x004058C1) to perform a sub-check or intersection test, passing calculated grid coordinates and a pointer from a local data structure. Its boolean return value (in al) controls whether further evaluation occurs.
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401D31  
  Called with floating-point parameters, an index (`ecx`), and a resource offset (`ebx+1A20h`) to query or test specific spatial/numeric properties; its boolean result (`al`) gates subsequent item configuration.
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x0040228B  
  Called inside a loop (clamping index up to 2 iterations) passing a computed index and a pointer to offset 0x1A20 of the resource block to query or validate grid/spatial status.

### CreateAndRegisterResourceEntity - `0x00402560`

_original name `sub_402560` | `void* __fastcall CreateAndRegisterResourceEntity(void *resourceBlock, int unused_edx, int type_index, int sub_variant_flag, int flags)` | confidence: high | complete disassembly and clear interaction with known resource subsystems and container managers | source: llm_

**Allocates, initializes, and configures a new resource or RTS object entity based on type lookup tables, coordinates, and flags, registering it into spatial grid and resource tracking containers.**

The function begins by bounds-checking the requested entity type index and variant flag against static lookup tables in .rdata (offset 0x41B178). Based on bitflags found in the lookup entry, it allocates a custom memory block of varying sizes using `AllocateCustomBlock` and initializes it via one of several specialized object initialization functions (`RtsObject_InitSpecializedComponent_410720`, `RtsObject_InitializeDerivedType`, `RtsObject_InitializeSubVariant`, or `RtsObject_InitializeBase`). Structured exception handling (SEH) is set up to clean up the allocated block via `FreeResourceBlock` if any subsequent initialization step fails. Once instantiated, the function calls `AllocateResourceIndexOrNode` to assign a resource index. It then copies coordinate bounds from the input buffer, updates grid flags via `UpdateResourceGridFlags` if coordinate scaling/clamping is needed, configures the block using `ConfigureResourceBlock`, appends the new entity pointer to the global resource container (growing its capacity via `GrowContainerCapacity` if necessary), inserts it into the spatial hash grid via `InsertIntoSpatialGrid`, and finally registers the entry with `RegisterResourceEntry` before returning the newly created entity pointer.

**Parameters:**
- `resourceBlock` (void*, ecx) - Pointer to the main resource block or manager structure.
- `type_index` (int, edx) - Index representing the entity or component type.
- `sub_variant_flag` (int, [esp+24h+arg_4]) - Sub-variant or category flag (must be <= 5).
- `flags` (int, [esp+24h+arg_C]) - Additional configuration flags influencing initialization and grid placement.

**Returns:** Pointer to the newly created and registered resource entity in eax, or NULL on failure.

**Side effects:**
- Allocates memory blocks via AllocateCustomBlock
- Modifies resource container arrays and spatial hash grids
- Updates global resource statistics and registry tables

**Data structures:**
- Resource block / Manager: +0x1A20 grid/flag data, +0x43D7Ch spatial grid, +0x51E80 resource index container, +0x5738C resource entry container
- Resource entity: +0x8/0xC coordinate bounds, +0x18 flags, +0x204/0x230 position fields

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401D64  
  Called following a successful spatial query to configure or initialize a specific sub-element with a constant flag (`5`) and the resource block (`ebx`), returning an object pointer whose field at offset `0x2B8` is updated.
- `ExecuteSpawnEntityScriptOpcode` (0x0040A3F0) - call at 0x0040A509  
  Called with parameters including global state (esi), integer bounds/indices (ebx, edi), and a stack-allocated coordinate/value buffer (var_8) to retrieve an object or resource pointer, whose field at offset +4 is used as the final result if non-null.
- `ProcessRtsResourceSpawningOrPathNodes` (0x00407B50) - call at 0x00407F3B  
  Called within an inner sub-loop (passing context pointers and index constants on the stack) to query, allocate, or update resource sub-elements based on the processed geometric/numeric parameters.
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x0040211B, 0x004022ED, 0x00402537  
  Called at multiple places (0x0040211B, 0x004022ED, 0x00402537) with varying arguments and stack/register configurations to retrieve, store, or update data elements associated with the resource block (ebp).

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004025BA, 0x004025E7, 0x00402612, 0x0040263B
- `RtsObject_InitSpecializedComponent_410720` (0x00410720) - call at 0x004025D7
- `RtsObject_InitializeDerivedType` (0x0040E760) - call at 0x00402602
- `RtsObject_InitializeSubVariant` (0x0040F880) - call at 0x0040262F
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x00402653
- `AllocateResourceIndexOrNode` (0x00418730) - call at 0x0040266F
- `ConvertAndClampCoordinate` (0x00414AA0) - call at 0x004026E3, 0x004026F1
- `UpdateResourceGridFlags` (0x00414DC0) - call at 0x0040270F
- `ConfigureResourceBlock` (0x0040B790) - call at 0x00402722
- `GrowContainerCapacity` (0x00404100) - call at 0x00402758
- `InsertIntoSpatialGrid` (0x004175E0) - call at 0x00402773
- `RegisterResourceEntry` (0x004042F0) - call at 0x0040278F
- `FreeResourceBlock` (0x00401000) - tail jump at 0x0041A3B3, 0x0041A3BB, 0x0041A3C3, 0x0041A3CB
- `__security_check_cookie` (0x004198F2) - call at 0x0041A3DC
- `__CxxFrameHandler3` (0x0041A224) - tail jump at 0x0041A3E6

### RegisterResourceEntry - `0x004042F0`

_original name `sub_4042F0` | `void __thiscall RegisterResourceEntry(void *this, int arg0_flag, void *base_ptr)` | confidence: medium | Based on register usage patterns, custom calling conventions, and clear memory access offsets, though exact struct types are inferred from usage context. | source: llm_

**Increments usage counters and updates statistical or positional fields for a resource sub-element based on lookup criteria and flags.**

The function operates using a __thiscall convention where `ecx` points to a main context or object structure. It reads a byte at offset 0x5Ch and a pointer/dword at offset 0x1Ch from `ecx`. It checks a boolean flag passed on the stack (`arg0_flag` at `[esp+4]`) to branch between two update paths. Depending on the branch, it calculates an address using `eax` (supplied via register) combined with the byte from `ecx+5Ch` scaled by 2, adding to a word counter. If the flag is non-zero, it further inspects a byte at `[edx+2Eh]` (where `edx` comes from `ecx+1Ch`), sign-extends it, and conditionally adds or subtracts it from an offset (`+0x0Ch` or `+0x10h`) relative to `eax`. Finally, if the low bit of a flags byte at `[edx]` is set (test bit 1), it increments a 32-bit counter at `[eax+0F4h]` before returning.

**Parameters:**
- `this` (void*, ecx) - Pointer to the parent context or resource object.
- `arg0` (int, [esp+4]) - Flag determining which increment path to take.
- `base_ptr` (void*, eax) - Base pointer to the target resource entry structure being updated.

**Returns:** none

**Side effects:**
- Increments a 16-bit word counter at a calculated offset from base_ptr
- Conditionally adds/subtracts a signed value from fields at base_ptr+0x0C or base_ptr+0x10
- Conditionally increments a 32-bit counter at base_ptr+0xF4

**Data structures:**
- Context: +0x1C void*, +0x5C unsigned char
- ResourceEntry: +0x0C int, +0x10 int, +0xF4 int

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040278F  
  Performs final integration or linking of the newly created resource entry into the active processing lists using calculated indices and flags.

### InsertIntoSpatialGrid - `0x004175E0`

_original name `sub_4175E0` | `void __fastcall InsertIntoSpatialGrid(void *entityStruct, int *gridArray)` | confidence: high | the function clearly implements a standard singly- or doubly-linked list bucket insertion using a spatial grid index calculated from entity coordinates. | source: llm_

**Calculates the grid cell index for an entity using its 2D coordinates and inserts it into a linked list at that cell position in a spatial hash grid.**

The function evaluates the entity's position by calling sub_417410 (CalculateGridIndex) using two floating-point values located at [esi+8] and [esi+12]. It stores the resulting grid index at [esi+248h] and clears a status or pointer field at [esi+244h]. It then accesses a grid bucket array pointed to by ebx using the grid index as a scaled lookup, retrieves the current head of the linked list at that bucket, and stores it in [esi+240h]. If an existing entity is present at the head of the bucket, its back-pointer at offset +0x244h is updated to point to the current entity (esi). Finally, the current entity is set as the new head of the grid bucket array at [ebx + index * 4].

**Parameters:**
- `entityStruct` (void*, esi) - Pointer to the entity structure being registered or updated in the grid, containing coordinate data starting at offset +8.
- `gridArray` (int*, ebx) - Pointer to the base of the grid lookup table / bucket array.

**Returns:** none

**Side effects:**
- Updates entity fields at offsets +0x240h, +0x244h, and +0x248h
- Modifies the linked list head in the grid bucket array at [ebx + index * 4]
- Updates the previous/next links of neighboring entities in the spatial grid bucket

**Data structures:**
- Entity: +0x8 float posX, +0xC float posY, +0x240h Entity *next, +0x244h Entity *prev, +0x248h int gridIndex

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00402773  
  Invoked with an offset into the base resource block (arg_0 + 0x43D7C) as part of post-registration management or synchronization.

**Calls:**
- `CalculateGridIndex` (0x00417410) - call at 0x004175E4

### CalculateGridIndex - `0x00417410`

_original name `sub_417410` | `int __cdecl CalculateGridIndex(float x, float y)` | confidence: medium | Based on FPU math operations, clamping logic against limit 0x17 (23), and combination scaling characteristic of grid/cell mapping routines. | source: llm_

**Calculates a combined integer grid or cell index from 2D floating-point coordinates by scaling, rounding via __ftol2_sse, clamping to bounds [0, 23], and combining them.**

The function performs a coordinate mapping or grid discretization computation. It takes floating-point inputs (passed via the x87 FPU stack), divides/multiplies them by constant factors (such as the constant at 0x0042ACBC), and converts the resulting floating-point values to integers using __ftol2_sse. It handles truncation/rounding adjustments by inspecting FPU status flags (fnstsw/test ah, 41h). The resulting integer coordinates are clamped to the range [0, 23] (0x17). Finally, it combines the two clamped indices using linear scaling (eax = esi + (eax * 3) * 8, representing a 2D-to-1D index computation or grid bucket lookup) and returns the resulting combined integer index in EAX.

**Parameters:**
- `x` (float, x87 FPU stack (st(0))) - First floating-point coordinate or value.
- `y` (float, x87 FPU stack (st(1) / st(2))) - Second floating-point coordinate or value.

**Returns:** The computed integer index/identifier in eax.

**Called by:**
- `InsertIntoSpatialGrid` (0x004175E0) - call at 0x004175E4  
  Called once to query or allocate an identifier/index, passing `esi+8` as a parameter (via `edi`). The returned index in `eax` is subsequently used to index into an array pointed to by `ebx`, store state values into `esi`, and update linked list pointers.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00417427, 0x00417462

### GrowContainerCapacity - `0x00404100`

_original name `sub_404100` | `void __stdcall GrowContainerCapacity(Container *container)` | confidence: high, clear logic, simple register-based conventions, and standard dynamic array growth pattern. | source: llm_

**Grows the capacity and reallocates the underlying data buffer of a container structure when it reaches capacity.**

The function calculates a new capacity for a dynamic array or container structure passed via its argument. It first checks if the current capacity (stored at offset +8) is zero; if so, it defaults to an initial capacity of 0x10. If non-zero, it doubles the current capacity (`ebx = eax * 2`). It then enters a loop that repeatedly doubles `ebx` until it is greater than or equal to a minimum threshold specified in `ecx`. Once the target capacity is determined, it multiplies the capacity by 4 (assuming 4-byte elements) to get the byte size and calls ReallocateCustomBlock to resize the underlying data buffer located at offset +0. Finally, it updates the container structure with the new buffer pointer and the new capacity, and returns.

**Parameters:**
- `container` (Container*, [esp+4]) - Pointer to the container structure whose capacity is being grown.
- `min_capacity` (size_t, ecx) - Minimum required capacity threshold passed via ecx.

**Returns:** none

**Side effects:**
- Reallocates the container's internal data buffer, potentially moving memory and freeing the old block.
- Updates the container's buffer pointer and capacity fields.

**Data structures:**
- Container: +0x0 void* buffer, +0x8 size_t capacity

**Called by:**
- `CreateScriptVariable` (0x004027A0) - call at 0x00402833  
  Called when an internal array/buffer capacity limit is reached (when the current count equals the maximum), passing a pointer to the container/manager structure on the stack to grow or reallocate the underlying storage buffer.
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00402758  
  Grows or reallocates an internal tracking array/container located within the resource block when its capacity limit is reached before storing the new resource pointer.

**Calls:**
- `ReallocateCustomBlock` (0x00418670) - call at 0x00404130

### ReallocateCustomBlock - `0x00418670`

_original name `sub_418670` | `void* __fastcall ReallocateCustomBlock(void *old_ptr, size_t new_size)` | confidence: high | complete clarity on register usage, caller expectations, header offsets, and helper function delegation | source: llm_

**Reallocates a custom tracked memory block, growing or shrinking it while preserving existing data, or handles in-place resizing if the current block's pool bucket can accommodate the new size.**

The function acts as a custom `realloc` for memory blocks managed by `AllocateCustomBlock` and `FreeResourceBlock`. It accepts the old memory pointer in `esi` and the requested new size in `edi`. If `esi` is null, it simply delegates directly to `AllocateCustomBlock(new_size)`. Otherwise, it inspects the metadata header located right before the user payload (specifically checking `[esi-6]`). If the block type byte `[esi-6]` is not `0xFF` (indicating it belongs to a fixed-size pool class), it looks up the pool bucket's actual capacity using a lookup table at `0x41B150`. If the requested new size `edi` fits within this existing bucket capacity, it performs an in-place adjustment: it updates the stored size field at `[esi-4]`, adjusts global allocation statistics (`dword_42281C`) by the delta, and returns `esi` immediately without reallocation. If the new size exceeds the current pool capacity or if it is a large block (`0xFF`), it allocates a brand new block of size `edi` via `AllocateCustomBlock`. If allocation succeeds, it copies the minimum of the old size and the new size from the old block to the new block using `memcpy`, frees the old block via `FreeResourceBlock`, and returns the pointer to the new memory block.

**Parameters:**
- `old_ptr` (void*, esi) - Pointer to the existing custom memory block payload, or NULL for fresh allocation.
- `new_size` (size_t, edi) - The requested new size in bytes for the memory block.

**Returns:** Pointer to the newly allocated or resized memory block payload in eax, or NULL on allocation failure.

**Side effects:**
- Allocates memory via AllocateCustomBlock
- Frees the old memory block via FreeResourceBlock if reallocated
- Modifies global tracking statistic dword_42281C during in-place resizing
- Copies payload contents via memcpy

**Data structures:**
- Custom Block Header: -0x6 block type/flags byte, -0x4 size/length field, 0x0 payload pointer

**Called by:**
- `AppendToDynamicBuffer` (0x00408F70) - call at 0x00408FA8  
  Called when the buffer's current capacity ([ebx+8]) is insufficient to hold the incoming data (i.e., current write offset + size exceeds capacity). It reallocates the buffer block, taking the new capacity in edi (doubled from the old capacity, starting at 0x10 if zero) and the existing buffer pointer in esi, and returns a pointer to the newly allocated memory block in eax.
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x00419526  
  Reallocates or resizes the backing storage buffer for the structure, taking the new capacity/size in edi and old pointer in esi, and returning the new buffer pointer in eax.
- `ResizeStateCapacity` (0x00419800) - call at 0x00419832  
  Called to allocate or reallocate the underlying storage buffer for the dynamic array. It is passed the requested allocation size in bytes (computed in EDI as capacity multiplied by 48, based on scaling factors like lea edi, [ebx+ebx*2] and shifts) and returns a pointer to the newly allocated or reallocated memory block in EAX.
- `ResizeContainer` (0x004198B0) - call at 0x004198E0  
  Called to allocate or reallocate the underlying memory buffer for the growing collection, passing the requested byte size in edi (calculated as the new capacity multiplied by an element stride/size) and storing the resulting new buffer pointer returned in eax back into the collection structure.
- `GrowContainerCapacity` (0x00404100) - call at 0x00404130  
  Called to allocate a new, larger memory buffer when a container's capacity limit is reached. It passes the required allocation size in bytes (calculated as the new capacity multiplied by 4, derived from doubling the previous capacity or defaulting to 16) as an argument, and receives a pointer to the newly allocated block in eax, which is then stored into the container structure.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004186A5; tail jump at 0x00418676
- `memcpy` (0x0041A22A) - call at 0x004186BE
- `FreeResourceBlock` (0x00401000) - call at 0x004186C8

### FreeResourceBlock - `0x00401000`

_original name `sub_401000` | `void __cdecl FreeResourceBlock(void *ptr)` | confidence: high | Disassembly clearly implements a custom resource deallocation and free-list caching mechanism with explicit magic number checking and MSVCR80!free integration. | source: llm_

**Frees a custom memory block with an 'RM' header, updating global tracking statistics and either returning it to an internal free list or releasing it via free().**

The function first checks if the input pointer in eax is null; if so, it returns immediately. It then adjusts the pointer backward by 8 bytes to access a custom header (`eax - 8`). It verifies the magic bytes at the header start (`'RM'`, value 0x4D52); if the magic does not match, it returns without freeing. It reads a size value from `[eax+4]` and subtracts it from a global tracking counter (`dword_42281C`). It then checks a byte at `[eax+2]`. If this byte is 0xFF, it clears the header word and calls `free()` on the adjusted pointer block. Otherwise, it uses this byte as an index into a global array of free lists (`dword_422820`), unlinks or links the block into that freelist as a cached resource chunk rather than returning it directly to the system heap.

**Parameters:**
- `ptr` (void *, eax) - Pointer to the resource data payload (located 8 bytes after the custom block header).

**Returns:** none

**Side effects:**
- frees or caches the memory block pointed to by eax
- decrements global tracking counter at 0x0042281C
- modifies global free list pointers at 0x00422820

**Data structures:**
- ResourceHeader: +0x0 word magic ('RM'), +0x2 byte flags/index, +0x4 dword size/tracking value, +0x8 start of data payload

**Called by:**
- `main` (0x00401050) - tail jump at 0x0041A3F3  
  Used as a tail jump target in the exception handling/SEH epilog/cleanup path.
- `CreateAndRegisterResourceEntity` (0x00402560) - tail jump at 0x0041A3B3, 0x0041A3BB, 0x0041A3C3, 0x0041A3CB  
  Tail-jumped to by compiler-generated exception handling stubs to handle cleanup during stack unwinding.
- `ReallocateCustomBlock` (0x00418670) - call at 0x004186C8  
  Called at the end of a successful reallocation with the old buffer pointer (esi) passed in eax, acting as a deallocation or cleanup routine for the old backing storage block after its contents have been copied to the new block.

### ConfigureResourceBlock - `0x0040B790`

_original name `sub_40B790` | `void __cdecl ConfigureResourceBlock(void *resourceBlock)` | confidence: high | Based on structural access patterns, calls to resource property evaluators, and integration with the resource factory initialization caller sub_402560. | source: llm_

**Initializes, computes dimensions and properties for, and configures a newly created resource block (passed in ebx) using lookup tables, resource property lookups, and virtual method calls.**

The function performs an extensive series of floating-point computations, lookups, and virtual function invocations on a resource block structure passed via the register `ebx`. It retrieves a sub-structure at offset `[ebx+1C_h]` (referred to as `esi`) containing various word and byte fields representing configuration attributes, dimensions, and flags. It utilizes helper functions `LookupResourceProperty` (sub_404460) and `EvaluateResourceProperty` (sub_4043B0) to query static configuration tables in `.rdata` against these resource properties. It also iterates through a lookup table (`unk_41CB18`) to match specific types and flags, applies conditional scaling factors, and invokes virtual methods via function pointers stored in the resource block vtable (such as `[ebx]` at offset `+1Ch` and child node traversal via `[ebx+20Ch]`). Finally, it updates status flags in the resource block header and cleans up the floating-point stack before returning.

**Parameters:**
- `resourceBlock` (void *, ebx) - Pointer to the resource block being initialized and configured.

**Returns:** none

**Side effects:**
- Modifies fields within the resource block passed in ebx, including status and configuration flags.
- Invokes virtual methods on the resource block and its linked child structures.
- Accesses global data lookup tables in .rdata.

**Data structures:**
- ResourceBlock: +0x0 vtable pointer, +0x18/1Ch bitflags/word fields, +0x1Bh byte flag, +0x20h data array/buffer, +0x20Ch child resource pointer (+0x1Ch next pointer)

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00402722  
  Performs initialization or registration on the newly created resource block (passed via ebx) before it is added to internal arrays.

**Calls:**
- `LookupResourceProperty` (0x00404460) - call at 0x0040B805
- `EvaluateResourceProperty` (0x004043B0) - call at 0x0040B8B2

### EvaluateResourceProperty - `0x004043B0`

_original name `sub_4043B0` | `float __cdecl EvaluateResourceProperty(void *resourceBlock)` | confidence: medium | control flow is clear, but exact data structure layout and table semantics are inferred from index loops and constant addresses | source: llm_

**Evaluates a resource block's configuration data and lookup tables to compute a scaling factor or dimension, returning the resulting floating-point value.**

The function inspects a structure passed as an argument, checking flags at offset +2 and +0x3Bh, and iterates through a lookup table in read-only memory spanning from 0x0041CB18 to 0x0041CB98 with 16-byte strides. It matches condition codes based on table entries and index values, accumulating indices into `esi` or `eax`. It then combines these index values with a byte field at `[ebp+0Dh]`, performs multiplication, converts the integer result via `fild`, multiplies it by a floating-point constant from `.rdata` (0x0041D958), and returns the resulting float via `st(0)`.

**Parameters:**
- `resourceBlock` (void *, [esp+4]) - Pointer to the resource block or configuration structure being evaluated.

**Returns:** st(0) (floating-point value)

**Data structures:**
- `ResourceBlock`

**Called by:**
- `ConfigureResourceBlock` (0x0040B790) - call at 0x0040B8B2  
  Called after processing a sequence of field conversions and multiplications involving the resource block's configuration data, acting as a secondary conversion or evaluation helper step in the property setup sequence.

### LookupResourceProperty - `0x00404460`

_original name `sub_404460` | `float __stdcall LookupResourceProperty(int arg_0)` | confidence: medium | Based on structural table-walking and x87 floating-point conversion/arithmetic typical of configuration sizing helpers. | source: llm_

**Searches a static configuration table in read-only data against properties of a given resource structure to compute or retrieve a floating-point resource parameter.**

The function inspects bytes at offsets +2 and +3Bh of a structure pointer passed via register esi (implicitly or through context). It loops through a static table located in .rdata starting at 0x0041CB18 (with entries spaced by 16 bytes up to 0x0041CB98), comparing table entries against the extracted byte values to find a matching configuration record. If a match is found and additional conditions on offset +3Bh are met, it indexes into a secondary table based on an input argument, extracts a value, converts it to floating-point, performs arithmetic operations on the x87 floating-point stack (fild, fadd st, st, fld), and returns a floating-point result via st(0). Otherwise, it falls back to a default zero-initialized floating-point calculation path.

**Parameters:**
- `arg_0` (int, [esp+4]) - An index or configuration value used to access secondary lookup tables when a match is found.

**Returns:** st(0) (floating-point value)

**Data structures:**
- ResourceBlock: +0x2 byte flag/type, +0x3Bh byte status/type

**Called by:**
- `ConfigureResourceBlock` (0x0040B790) - call at 0x0040B805  
  Called during the initial series of floating-point and integer sizing calculations on fields extracted from the resource block's structure (`[ebx+1Ch]`), passing an adjusted integer index/value as an argument (pushed via edi + ebp) to perform a lookup or conversion helper operation.

### UpdateResourceGridFlags - `0x00414DC0`

_original name `sub_414DC0` | `void __fastcall UpdateResourceGridFlags(void *resourceBlock, int unused_edx, int param_ecx, int param_eax, int param_esi, char flag_bl)` | confidence: high, clear register-based 2D iteration and bit-manipulation loop with well-defined structural offsets. | source: llm_

**Iterates over a 2D coordinate range (grid/map sub-structure) to set or clear specific flag bits within a data table.**

The function takes coordinate and range parameters via registers (eax, ecx, esi) and a pointer via stack (arg_0), along with a boolean flag in bl. It centers/adjusts the coordinate spans, computes a base pointer offset into a large grid/array structure at `arg_0 + 0xB704`, and loops over a 2D region bounded by the calculated dimensions. For each grid cell within bounds (0 to 0x60), it checks bounds and either sets bits (`0x13`) or clears bits (`0xEC`) depending on `bl`. Finally, it increments a counter at `arg_0 + 0xE748` before returning.

**Parameters:**
- `resourceBlock` (void*, arg_0 [esp+0Ch]) - Main resource/grid block pointer containing data arrays and counters.
- `param_eax` (int, eax) - Width or X coordinate parameter used for centering and span calculation.
- `param_ecx` (int, ecx) - Height or Y coordinate parameter used for centering and row iteration.
- `param_esi` (int, esi (arg_4)) - Additional coordinate or size parameter.
- `flag_bl` (char, bl) - Boolean flag determining whether to set or clear bits in the grid table.

**Returns:** none

**Side effects:**
- Modifies bytes in a grid table located at resourceBlock + 0xB704 based on coordinate ranges and bl flag
- Increments a 32-bit counter at resourceBlock + 0xE748

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040270F  
  Called with geometry/coordinate parameters and flags to transform or store spatial data into a sub-structure at offset 0x1A20 of the main resource block.

### ConvertAndClampCoordinate - `0x00414AA0`

_original name `sub_414AA0` | `int __cdecl ConvertAndClampCoordinate(double val)` | confidence: high | standard mathematical conversion and clamping routine using x87 FPU and standard bounds check | source: llm_

**Scales a floating-point coordinate value, converts it to an integer, and clamps the result to the range [0, 95].**

The function loads a floating-point value (presumably passed on the x87 FPU stack), multiplies it by a constant factor stored in .rdata (0x0041D960), and converts the result to a 32-bit signed integer via __ftol2_sse. It then performs a floor/rounding check by comparing the integer back to the floating-point value and adjusting downward if necessary. Finally, it clamps the resulting integer to the inclusive range [0, 95] (0x5F) and returns it in EAX.

**Parameters:**
- `val` (double, st(0)) - Floating-point coordinate or dimension value to scale and clamp.

**Returns:** eax, integer clamped to [0, 95]

**Called by:**
- `ExecuteSpawnEntityScriptOpcode` (0x0040A3F0) - call at 0x0040A487, 0x0040A497  
  Called twice in succession to obtain coordinate or dimension values, using intermediate floating-point calculations to prepare arguments for a subsequent manager/resource lookup.
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x004026E3, 0x004026F1  
  Called twice in succession to retrieve coordinate or numerical values used to configure spatial properties when a specific flag is set in the table entry.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00414AB4

### AllocateResourceIndexOrNode - `0x00418730`

_original name `sub_418730` | `int __fastcall sub_418730(void *pContainer, void *pResource)` | confidence: high | clear register-based container manipulation and indexing logic matching pool/allocator patterns | source: llm_

**Performs a bounds check on an array/pool structure, and if valid, allocates an element by updating indices, copying handles, and incrementing an allocation counter.**

The function expects a container pointer in `eax` and an additional parameter `pResource` in `esi`. It checks if the current index `[eax+8]` is less than the upper limit `[eax+4]`. If the index has reached or exceeded the limit, it returns 0 (indicating failure/exhaustion). If valid, it computes an entry address from a base array pointer `[eax]` plus the scaled current index, stores `esi` into the entry, updates the container's current index from a field in the entry (`[ecx+6]`), increments a count at `[eax+0Ch]`, and constructs a return value combining a 16-bit field from `[ecx+4]` shifted left by 16 with the incremented index value, storing it in `[esi+4]` before returning non-zero.

**Parameters:**
- `pContainer` (void*, eax) - Pointer to the container or pool structure containing bounds, current index, and array base.
- `pResource` (void*, esi) - Pointer to a resource or sub-element structure being registered/allocated.

**Returns:** Returns a combined 32-bit ID/status value in eax (composed of a 16-bit word shifted left by 16 combined with the current index), or 0 if bounds check fails.

**Side effects:**
- Modifies fields within the container structure (current index at +0x8, count at +0xC)
- Modifies fields within the resource structure passed in esi (+0x0, +0x4)

**Data structures:**
- Container: +0x0 base pointer to array of entries, +0x4 upper limit index, +0x8 current index, +0xC allocation count
- Entry: +0x0 pointer/value storage, +0x4 16-bit value, +0x6 16-bit next/limit index

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040266F  
  Performs a validation or guard check on a sub-element of the base resource structure (arg_0 + 0x51E80); if it returns non-zero, the initialization is aborted and cleanup/exit occurs.

### RtsObject_InitializeSubVariant - `0x0040F880`

_original name `sub_40F880` | `__thiscall void RtsObject_InitializeSubVariant(void *this, int type_index, int flags)` | confidence: medium | Based on callee context, register usage (esi as this), and field initializations. | source: llm_

**Initializes a specialized resource sub-element variant by calling the base initializer and configuring specific floating-point and scalar state fields.**

The function begins by calling sub_40B100 [RtsObject_InitializeBase] to perform standard base initialization of the object passed via the `esi` register. It then sets up various fields on the object, including installing a specific virtual table at offset +0x0 (offset off_41D3A0), setting an integer field at +0x29C and +0x2A8 to zero, and clearing a byte at +0x2B4. It extracts a value from a lookup structure retrieved via offset +0x1C of the object and stores it, alongside performing several floating-point load and store operations to initialize internal coefficients or parameters.

**Parameters:**
- `this` (void*, esi) - Pointer to the resource sub-element object being initialized.
- `type_index` (int, stack (passed via caller setup before sub_40B100)) - Type index parameter forwarded to the base initializer.
- `flags` (int, stack (passed via caller setup before sub_40B100)) - Flags parameter forwarded to the base initializer.

**Returns:** none

**Side effects:**
- Initializes the object's virtual table pointer at offset +0x0.
- Modifies multiple fields within the object structure, including offsets +0x29C, +0x2A8, and +0x2B4.

**Data structures:**
- RtsObject: +0x0 vtable (offset off_41D3A0), +0x1C pointer to config/lookup struct, +0x29C int field, +0x2A8 int field, +0x2B4 byte field

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0040262F  
  Invoked with arg_4 and index parameters when the tertiary allocation flag branch is taken, performing specific setup for that sub-element variant.

**Calls:**
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x0040F884

### RtsObject_InitializeDerivedType - `0x0040E760`

_original name `sub_40E760` | `void* __fastcall RtsObject_InitializeDerivedType(void *obj, int type_index, int flags)` | confidence: high, clear structure initialization logic following a base initializer call and static table lookup. | source: llm_

**Initializes a specific derived RTS object instance, setting up its virtual table, searching a static lookup table to configure its type properties, and initializing internal state fields.**

The function begins by calling RtsObject_InitializeBase to perform baseline initialization of the allocated object instance (setting up its vtable and basic sub-objects). It then installs a derived type-specific virtual table pointer (`off_41D374`) at the start of the object structure. Following this, it iterates through a static table starting at `asc_41BD00` (inspecting elements in 0x20-byte steps) to match a property against the passed `type_index` in `edi`. If a match is found, the matched index is scaled and used to retrieve specific configuration parameters; otherwise, fallback default values are used. Finally, it initializes various member fields offset in the object structure from `+0x24C` to `+0x278` to default states (such as zeroing counters, setting flags to 1, and clearing pointers/buffers), and returns the initialized object pointer in `eax`.

**Parameters:**
- `obj` (void*, esi) - Pointer to the newly allocated RTS object structure being initialized.
- `type_index` (int, edi) - Type index or identifier used to search the lookup table.
- `flags` (int, pushed on stack / arg_4) - Flags passed through to RtsObject_InitializeBase.

**Returns:** Returns the initialized object pointer in eax.

**Side effects:**
- Initializes memory fields of the target object instance passed in esi.

**Data structures:**
- RtsObjectDerived: +0x0 vtable (off_41D374), +0x24C int state/counter, +0x250 int flag, +0x254 int/ptr, +0x258 int flag, +0x260-0x278 zeroed/initialized fields.

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00402602  
  Invoked with an argument (arg_4) when the secondary allocation flag branch is taken, configuring the newly allocated resource structure.

**Calls:**
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x0040E763

### RtsObject_InitSpecializedComponent_410720 - `0x00410720`

_original name `sub_410720` | `void* __fastcall RtsObject_InitSpecializedComponent_410720(void *esi, int unused, int arg_4)` | confidence: high | standard component initialization pattern matching nearby functions | source: llm_

**Initializes a specialized RTS game object or component structure by setting up its virtual method table, initializing base sub-objects, and configuring several default numeric and pointer fields.**

The function begins by calling `sub_40B100 [RtsObject_InitializeBase]` to initialize the base object at `esi` with type index `0x27`. It then performs multiple floating-point load/store operations (presumably resetting or initializing floating-point attributes) and sets up the object's virtual table pointers (`offset off_41D3A0` and later `offset off_41D3CC`). It clears various internal fields of the object structure (offsets `+0x29C`, `+0x2A8`, `+0x2B4`, `+0x2BC`, `+0x2C4`), extracts a word from a sub-structure located at `[esi+1Ch]` at offset `+0x2C`, and assigns a default integer value (`0x30D4`) to offset `+0x2B8`.

**Parameters:**
- `obj` (void*, esi) - Pointer to the object or component being initialized.
- `arg_4` (int, esp+4) - Additional argument passed from caller.

**Returns:** none

**Side effects:**
- Initializes the memory block pointed to by esi as a specialized game object/component
- Sets virtual table pointers and clears/sets numerous internal fields up to offset 0x2C4

**Data structures:**
- Object structure: +0x0 vtable pointer, +0x1C pointer to sub-structure (which contains a word at +0x2C), +0x29C int field, +0x2A8 int field, +0x2B4 char/byte field, +0x2B8 int field (set to 0x30D4), +0x2BC int field, +0x2C4 int field

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x004025D7  
  Invoked with an argument (arg_4) when the primary allocation flag indicates a specific sub-type, performing specialized setup or initialization on the allocated resource.

**Calls:**
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x00410725

### RtsObject_InitializeBase - `0x0040B100`

_original name `sub_40B100` | `void* __stdcall RtsObject_InitializeBase(void *obj, int type_index, int flags)` | confidence: high | complete disassembly and caller context provide precise structural understanding. | source: llm_

**Initializes a newly allocated resource or sub-element structure by setting up its virtual table, clearing its state fields, and iterating through a lookup table to create and register associated component sub-objects via the component factory.**

The function begins by setting up a structured exception handling (SEH) frame and initializing the target object pointer passed in `ebx`. It sets the object's virtual table pointer to `off_41D348`, clears various fields and flags (such as zeroes and default states), and stores input parameters (`type_index` and `flags`) into specific structure offsets. It then checks if `type_index` exceeds 0x30, using a lookup table (`0x0041B178`) to retrieve property words and configuration data. Next, it enters a loop running up to 4 iterations (`ebp` from 0 to 3) that reads configuration bytes from the selected lookup table entry; if a valid component type is specified (not `0xFF`) and the internal count is below 5, it calls `RtsComponentFactory_Create` to instantiate a sub-component, stores the resulting pointer in an array field (`+0x210` onwards), increments the count, and invokes the sub-component's virtual initialize method (`[edx+4]`). Finally, it restores the previous stack frame and exception handler, returning the initialized object pointer.

**Parameters:**
- `obj` (void*, [esp+28h+arg_0] (passed in ebx)) - Pointer to the newly allocated object structure being initialized.
- `type_index` (int, [esp+28h+arg_4]) - Index or identifier used to look up configuration and sub-component requirements.
- `flags` (int, [esp+28h+arg_8] (loaded into al)) - Configuration flags or attributes stored into the object structure.

**Returns:** The initialized object pointer (in eax, equal to `obj`).

**Side effects:**
- Modifies the memory block pointed to by `obj`, populating its virtual table pointer, clearing fields, and setting configuration bytes.
- Allocates and initializes up to 4 sub-component objects by calling `RtsComponentFactory_Create`.

**Data structures:**
- Object structure: +0x0 vtable (`off_41D348`), +0x8 / +0xC data fields, +0x18 / +0x1A / +0x1B state flags and bytes, +0x1C lookup table pointer, +0x5C type index byte, +0x5E component count, +0x210 array of sub-component pointers (up to 4 entries).

**Called by:**
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x00402653  
  Called with the allocated object pointer, an index, and another parameter when the default allocation branch is selected, initializing the fallback resource structure.
- `RtsObject_InitializeSubVariant` (0x0040F880) - call at 0x0040F884  
  Called immediately upon entering the initialization sequence to perform a preparatory setup step on the object or context, receiving object and stack-based arguments pushed prior to the call.
- `RtsObject_InitializeDerivedType` (0x0040E760) - call at 0x0040E763  
  Called unconditionally at the very beginning of the function (with `esi` presumed to hold the object pointer) as part of the object's initialization or base setup phase before its member fields and vtable are populated.
- `RtsObject_InitSpecializedComponent_410720` (0x00410720) - call at 0x00410725  
  Called as part of the object initialization sequence for `esi`, receiving `esi` in a register/stack argument along with the constant 0x27 and other preliminary values, likely acting as a base constructor, sub-component allocator, or property initialization routine.

**Calls:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0040B2AB
- `__security_check_cookie` (0x004198F2) - call at 0x0041A394
- `__CxxFrameHandler3` (0x0041A224) - tail jump at 0x0041A39E

### RtsComponentFactory_Create - `0x00411580`

_original name `sub_411580` | `void* __fastcall RtsComponentFactory_Create(void *arg_ebx, int arg_edx, int type_index, int arg_edi)` | confidence: high, clear factory dispatcher pattern with comprehensive callee references and explicit jump table bounds checking. | source: llm_

**Factory function that allocates and initializes various RTS component or sub-component objects based on a type index lookup table.**

The function acts as a centralized component factory. It first checks if the input type index in `esi` (or parameters) falls within a valid bounds range (0 to 0x17). It then maps the index through a byte lookup table `byte_41BDC0` using an indexed multiplication calculation. The resulting byte value serves as a case selector (0 through 15) dispatched via a jump table (`jpt_4115A9`). For each valid case, it allocates a custom memory block of a specific size (ranging from 0x1C to 0x28 bytes) using `AllocateCustomBlock`, and upon successful allocation, invokes the corresponding constructor or initialization function for that component type, passing appropriate parameters in registers and on the stack (`edi`, `esi`, `ebx`). If the index is out of bounds or allocation fails, it safely returns 0 (null).

**Parameters:**
- `arg_ebx` (void*, ebx) - Context or manager object passed as an argument to component constructors.
- `type_index` (int, esi) - Component type index used to lookup the factory case.
- `arg_edi` (int, edi) - Additional parameter forwarded to the specific component constructor/initializer.

**Returns:** Pointer to the newly allocated and initialized component instance in eax, or NULL (0) on failure.

**Side effects:**
- Allocates memory blocks from the custom block allocator or heap.

**Called by:**
- `RtsManager::UpgradeComponent` (0x0040F3D0) - call at 0x0040F465  
  Called when an existing matching component is not found in the manager's array (`ebx+210h`), to allocate or initialize a new component object; the resulting pointer is stored into the array and its virtual method at offset +4 is invoked.
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x0040B2AB  
  Called inside a loop (iterating 4 times) to instantiate or fetch a sub-resource when a table entry check succeeds (`cmp al, 0FFh` and state check `cmp byte ptr [ebx+5Eh], 5`). If successful, the returned object pointer is stored into an array within the main structure (`[ebx+ecx*4+210h]`), the count at `[ebx+5Eh]` is incremented, and a virtual method at offset +4 of the returned object is invoked.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004115B5, 0x004115D1, 0x004115EE, 0x0041160B, 0x00411628, 0x00411645, 0x00411662, 0x0041167F (+8 more)
- `RtsSubComponentA_Init` (0x00413870) - call at 0x004115C6
- `CComponentType2Factory_Constructor` (0x00414150) - call at 0x004115E3
- `RtsComponentType3_Constructor` (0x004117B0) - call at 0x00411600
- `RtsComponentCase4_Init` (0x004119C0) - call at 0x0041161D
- `FactoryCase5Constructor` (0x00411CB0) - call at 0x0041163A
- `RtsComponentType6_Init` (0x00411DD0) - call at 0x00411657
- `RtsComponent7_Constructor` (0x004121B0) - call at 0x00411674
- `RtsComponentType8_Constructor` (0x00412330) - call at 0x00411691
- `SubComponentType9_Constructor` (0x004124B0) - call at 0x004116AE
- `RtsComponent10_Constructor` (0x00412950) - call at 0x004116CB
- `RtsComponentType11_Init` (0x00412C80) - call at 0x004116E8
- `RtsSubComponent12_Constructor` (0x00412D40) - call at 0x00411701
- `FactoryItem13_Constructor` (0x00412E50) - call at 0x0041171A
- `InitializeComponentType14` (0x00413050) - call at 0x00411733
- `RtsSubComponent15_Constructor` (0x00413390) - call at 0x0041174C
- `ComponentTypeInfo_Initialize` (0x004137C0) - call at 0x00411765

### ComponentTypeInfo_Initialize - `0x004137C0`

_original name `sub_4137C0` | `void __fastcall ComponentTypeInfo_Initialize(void *this_obj, int edx_unused, int type_index, char arg0)` | confidence: high | standard component initialization pattern with vtable setup and lookup table indexing | source: llm_

**Initializes a component descriptor/configuration object (`this` in `eax`) using a lookup table (`byte_41BDC0`) indexed by a validated `type_index` (`ecx`), and sets up virtual method table pointers.**

The function initializes a newly allocated structure pointed to by `eax` (acting as the `this` pointer). It stores `edx` at `[eax+8]`, clears an FPU register via `fldz` / `fst` / `fstp`, and validates/clamps the type index passed in `ecx` against the limit 0x18 (24). It calculates a lookup offset into a global table (`byte_41BDC0`) based on this index, stores the resulting pointer at `[eax+0Ch]`, extracts a specific bit flag from the lookup table entry to store at `[eax+12h]`, and saves parameters `cl` (`arg0`) and `ecx` (`type_index`) into respective fields. During initialization, it temporarily sets the virtual table pointer at `[eax]` to `off_41D464` and finally overwrites it with `off_41D73C` before returning, cleaning up 4 bytes of stack parameters.

**Parameters:**
- `this` (void*, eax) - Pointer to the allocated object being initialized.
- `type_index` (int, ecx) - Index or type identifier used to look up metadata in byte_41BDC0.
- `arg0` (char, [esp+4]) - Additional configuration byte stored into the object.

**Returns:** none

**Side effects:**
- Initializes fields of the object at `eax`, including vtable pointers `off_41D464` and `off_41D73C`.

**Data structures:**
- Object struct layout: +0x0 vtable (off_41D73C), +0x4 dword (0), +0x8 edx value, +0x0C ptr to byte_41BDC0 entry, +0x10h type_index (cl), +0x11h arg0 (cl), +0x12h extracted table flag byte.

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411765  
  Called as the constructor/initializer for the final factory case, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsSubComponent15_Constructor - `0x00413390`

_original name `sub_413390` | `void __stdcall RtsSubComponent15_Constructor(void *this_obj, int param_ecx, int param_arg0)` | confidence: high | based on clear structure initialization patterns, register usage matching the caller description, and table indexing logic. | source: llm_

**Initializes a specific sub-component object (case 15 factory instance) by setting up virtual tables, member fields, and bounds-checked lookup references.**

The function acts as a constructor/initializer for an object instance passed in `eax`. It initializes multiple fields of the object: setting a virtual method table at offset +0 (initially `off_41D464`, later overwritten by `off_41D708`), setting a zeroed field at +0x4, storing `edx` at offset +8, storing `cl` (derived from the lower byte of `arg_0` or `ecx`) at offsets +0x10 and +0x11, and performing conditional arithmetic and masking using a lookup table at `byte_41BDC0`. It computes a clamped/masked index based on `ecx` (comparing against 24 / 0x18), indexes into `byte_41BDC0`, extracts a bit flag shifted from that table entry, stores it at offset +0x12, and zeroes offset +0x13. Floating point stack operations (`fldz`, `fst`, `fstp`) are performed, likely as a compiler artifact or for initializing an internal float/double field.

**Parameters:**
- `this_obj` (void*, eax) - Pointer to the newly allocated object instance being initialized.
- `param_edx` (int, edx) - Value stored into the object at offset +8.
- `param_ecx` (int, ecx) - Index or parameter used for bounds checking and lookup calculation.
- `arg_0` (int, [esp+4]) - Additional parameter byte stored into the object at offset +0x11.

**Returns:** none

**Side effects:**
- Initializes fields and virtual function table pointers of the object passed in eax.

**Data structures:**
- SubComponent15 Object: +0x0 vtable*, +0x4 int (0), +0x8 int (edx value), +0x0C void* (lookup pointer), +0x10 char (cl), +0x11 char (arg_0 byte), +0x12 char (bit flag from table), +0x13 char (0)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0041174C  
  Called as the constructor/initializer for factory case 15, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### InitializeComponentType14 - `0x00413050`

_original name `sub_413050` | `void __fastcall InitializeComponentType14(void *this_obj, int edx_unused, int arg_ecx, int arg_stack)` | confidence: high | well-aligned with the caller's description as a factory constructor for case 14 | source: llm_

**Initializes a newly allocated component object (factory case 14) by setting up its virtual method table, initializing fields based on the provided index and argument, and performing lookup operations on a global configuration table.**

The function initializes a structure pointed to by `eax` (which appears to be `this`). It sets the virtual table pointer twice (initially to `off_41D464`, later finalized to `off_41D6D4`), stores `edx` at `+0x8`, clears `+0x4` to zero, and stores `cl` (derived from an argument) into `+0x10`. It performs range clamping or masking on `ecx` (using bit manipulation with `cmp ecx, 18h` and `setnl`), multiplies or uses it as an index to compute an offset into the global table `byte_41BDC0`, stores the resulting pointer at `+0xc`, extracts specific bit flags from the table entry (shifting and masking), and stores a derived boolean flag at `+0x12`, zeroing out `+0x13`. Floating-point operations (`fldz`, `fst`, `fstp`) are executed, possibly to initialize internal floating-point members or due to compiler optimization remnants.

**Parameters:**
- `this_obj` (void*, eax) - Pointer to the newly allocated component structure being initialized.
- `val_edx` (int, edx) - A value stored into the object at offset +0x8.
- `index_ecx` (int, ecx) - An index or type identifier used for configuration and table lookups.
- `arg_0` (unsigned char, [esp+4]) - An additional byte argument stored into the object at offset +0x11.

**Returns:** none

**Side effects:**
- Initializes memory fields and virtual table pointers of the object passed in eax
- Accesses global configuration tables byte_41BDC0 and virtual tables off_41D464/off_41D6D4

**Data structures:**
- ComponentType14: +0x0 vtable*, +0x4 unknown/zero, +0x8 pointer/int, +0xc table_ptr, +0x10 byte_val, +0x11 byte_val2, +0x12 flag_byte, +0x13 byte_zero

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411733  
  Called as the constructor/initializer for factory case 14, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### FactoryItem13_Constructor - `0x00412E50`

_original name `sub_412E50` | `void __fastcall FactoryItem13_Constructor(void *this_ptr, int unused, int arg_ecx, char arg_0)` | confidence: high | standard object constructor pattern with register-based arguments matching caller description | source: llm_

**Initializes a newly allocated component object (factory case 13) using parameters passed in registers and on the stack, setting up virtual table pointers and internal configuration fields.**

The function acts as a constructor/initializer for an object (likely a C++ class instance) pointed to by `eax` (though `eax` is loaded/written, the context implies `eax` holds the `this` pointer or is populated during allocation setup, let's examine standard usage). It sets up a virtual table pointer at offset +0 (`off_41D6A0`, overwriting an earlier setup at `off_41D464`), clears offset +4, stores `edx` at offset +8, and handles bounds/clamping logic on `ecx` relative to the value 0x18 to compute a lookup index into a data array (`byte_41BDC0`). It extracts configuration flags and bytes from the lookup table and the stack argument (`arg_0`), storing them across various offsets of the object structure (+0Ch, +10h, +11h, +12h, +13h). Finally, it cleans up a floating-point register state (`fldz`, `fst`, `fstp`) and returns, cleaning up 4 bytes from the stack.

**Parameters:**
- `this_ptr` (void*, eax) - Pointer to the memory block being initialized (object instance).
- `param_edx` (int, edx) - A value stored at offset +8 of the object.
- `param_ecx` (int, ecx) - An index/identifier used for clamping and lookup table offset calculations.
- `arg_0` (char, [esp+4]) - A byte argument stored at offset +11h of the object.

**Returns:** none

**Side effects:**
- Initializes fields and virtual method table pointers of the object structure pointed to by eax

**Data structures:**
- Object struct: +0x0 vtable pointer, +0x4 dword, +0x8 dword (param_edx), +0xC pointer (lookup table entry), +0x10 byte (clamped ecx), +0x11 byte (arg_0), +0x12 byte (extracted flag from table), +0x13 byte (zero)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0041171A  
  Called as the constructor/initializer for factory case 13, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsSubComponent12_Constructor - `0x00412D40`

_original name `sub_412D40` | `void __fastcall RtsSubComponent12_Constructor(void *obj, void *edx_unused, int arg_ecx, int arg_stack0)` | confidence: high | matches the factory pattern and register usage described by callers | source: llm_

**Initializes an object instance (case 12 sub-component) by setting up virtual method tables, storing configuration parameters, and calculating lookup properties from a static table.**

The function initializes a newly allocated object structure passed in eax. It stores various parameters into member fields: the edx register is written to offset +8, the cl byte (from ecx) to offset +10h, and an argument from the stack (`[esp+4]`) to offset +11h. It sets up the virtual method table pointer at offset 0 (initially pointing to `off_41D464`, then overwritten with `off_41D66C`), clears offset +4, and initializes a floating-point zero at offset +8/12 context. It performs range clamping/masking on `ecx` using bitwise operations (`cmp ecx, 18h`, `setnl`, `sub`, `and`), multiplies it to index into a static data table (`byte_41BDC0`), extracts a bitflag from that table entry via shifting and masking, and stores the resulting byte flag at offset +12h, while setting offset +13h to zero.

**Parameters:**
- `obj` (void*, eax) - Pointer to the newly allocated object block being initialized.
- `param_ecx` (int, ecx) - Identifier or size index value used for lookup configuration.
- `param_stack` (unsigned char, [esp+4]) - An additional byte parameter stored at offset +11h of the object.

**Returns:** none

**Side effects:**
- Initializes fields of the object structure pointed to by eax
- References static data tables off_41D464, off_41D66C, and byte_41BDC0

**Data structures:**
- SubComponent12 object: +0x0 vtable pointer, +0x4 dword (0), +0x8 pointer/value (edx), +0x0C table pointer, +0x10 byte (cl), +0x11 byte (stack arg), +0x12 byte (flag from table), +0x13 byte (0)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411701  
  Called as the constructor/initializer for factory case 12, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponentType11_Init - `0x00412C80`

_original name `sub_412C80` | `void __fastcall RtsComponentType11_Init(void *pThis, int edx_unused, int arg_ecx, int arg_0)` | confidence: high | standard component constructor pattern with table-driven initialization | source: llm_

**Initializes a specific RTS component object (factory case 11) using configuration data from a lookup table and storing various parameters.**

The function initializes an object instance located at `eax` (passed implicitly or via register context). It sets up a virtual function table pointer at offset 0 (`off_41D464`, later overwritten by `off_41D638`), clears offset +4, stores `edx` at offset +8, and saves `cl` (truncated `ecx`) at offset +10h. It performs a clamp/mask operation on `ecx` using a comparison against 0x18, multiplies the result by an element size, and computes a pointer into a global configuration table (`byte_41BDC0`), storing it at offset +0Ch. It extracts a bitfield value from the configuration table entry, saves it at offset +12h, stores the argument from `[esp+arg_0]` at offset +11h, clears offset +13h, and finally updates the vtable pointer to `off_41D638` before returning, cleaning up 4 bytes of stack parameters.

**Parameters:**
- `pThis` (void*, eax) - Pointer to the allocated object memory being initialized.
- `edx_value` (int, edx) - Value stored into field at offset +8.
- `arg_ecx` (int, ecx) - Index or parameter used to index into the lookup table byte_41BDC0.
- `arg_0` (unsigned char, [esp+4]) - Additional configuration byte stored at offset +11h.

**Returns:** none

**Side effects:**
- Initializes fields of the object structure pointed to by eax
- References global data tables off_41D464, off_41D638, and byte_41BDC0

**Data structures:**
- Object layout: +0x0 vtable ptr, +0x4 int (0), +0x8 void* / int, +0xC const void*, +0x10 char/byte, +0x11 char/byte, +0x12 char/byte, +0x13 char/byte

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004116E8  
  Called as the constructor/initializer for factory case 11, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponent10_Constructor - `0x00412950`

_original name `sub_412950` | `void* __fastcall RtsComponent10_Constructor(void *this_obj, int ecx, int arg0)` | confidence: high, based on the clear layout of constructor initialization code, vtable assignments, field offsets, and caller context. | source: llm_

**Initializes a specific RTS game component structure (factory case 10) by setting up its virtual method table, storing initialization parameters, and configuring internal flags based on a lookup table.**

The function initializes a memory block passed in `eax` (acting as the `this` pointer for a component object). It sets the virtual table pointer at offset 0 (temporarily `off_41D464`, then overwritten with `off_41D604`), stores `edx` at offset `+8`, and a zero at offset `+4`. It performs boundary/clamping logic on `ecx` using a threshold of `18h` (24), computes an offset into a global configuration/lookup array `byte_41BDC0`, and stores data extracted from that lookup table into various object fields. It also reads an argument from the stack (`arg_0`), packs various bit-field properties into bytes at offsets `+10h`, `+11h`, and `+12h`, clears offset `+13h`, and cleans up the floating-point stack (`fldz`, `fst`, `fstp`) before returning with a 4-byte stack cleanup.

**Parameters:**
- `this_obj` (void*, eax) - Pointer to the newly allocated component memory block to be initialized.
- `param_index` (int, ecx) - Type or index identifier passed from the factory dispatcher, validated against 0x18.
- `arg_0` (unsigned char, esp+4) - Additional configuration byte passed on the stack.

**Returns:** none

**Side effects:**
- Initializes the component object structure pointed to by eax
- Modifies fields within the object memory block from offset 0 to 0x13

**Data structures:**
- ComponentObject: +0x0 vtable ptr, +0x4 dword, +0x8 ptr/value, +0xC lookup ptr, +0x10 byte (index/clamped), +0x11 byte (arg_0), +0x12 byte (bitfield from lookup), +0x13 byte (zero)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004116CB  
  Called as the constructor/initializer for factory case 10, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### SubComponentType9_Constructor - `0x004124B0`

_original name `sub_4124B0` | `void* __stdcall SubComponentType9_Constructor(void *objectInstance, int index, char arg_0)` | confidence: high | Based on clear field initializations, vtable assignments, and caller context indicating a factory constructor. | source: llm_

**Initializes an object instance (allocated at `eax`) for factory case 9, setting up its virtual method table, boundary-checked configuration values, and reference data fetched from a lookup table (`byte_41BDC0`).**

The function acts as a specialized constructor for a specific sub-component type created by the factory dispatcher. It receives a pointer to the newly allocated object block in `eax`, an index value in `ecx`, and a parameter on the stack (`arg_0`). It initializes fields of the object structure at `eax`: stores an initial pointer/context in `[eax+8]`, sets an initial vtable pointer (`off_41D464`, which is later overwritten by `off_41D5D0`), zeros out fields at `[eax+4]` and `[eax+24h]`, stores the byte-clamped index into `[eax+10h]` (clamping `ecx` based on whether it is less than 0x18), calculates an offset into a global lookup table `byte_41BDC0` using scaled multiplication, extracts specific bitfields from the lookup table entry to populate configuration flags at `[eax+12h]`, stores `arg_0` at `[eax+11h]`, and zeroes out a trailing byte at `[eax+13h]`. It also performs an unused floating-point zero load (`fldz`) and clear operation via `fst`/`fstp`.

**Parameters:**
- `objectInstance` (void*, eax) - Pointer to the newly allocated object memory block being initialized.
- `index` (int, ecx) - Type or index identifier used for lookup table indexing and configuration.
- `arg_0` (char, [esp+4]) - An additional initialization byte parameter.

**Returns:** none

**Side effects:**
- Initializes memory pointed to by eax, setting vtables, configuration fields, and lookup table references.

**Data structures:**
- SubComponentType9 Object: +0x0 vtable/pointer, +0x4 dword (zeroed), +0x8 dword (edx context), +0x0C lookup table pointer, +0x10 clamped index byte, +0x11 arg_0 byte, +0x12 extracted flag byte, +0x13 byte (zeroed), +0x24 dword (zeroed)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004116AE  
  Called as the constructor/initializer for factory case 9, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponentType8_Constructor - `0x00412330`

_original name `sub_412330` | `void __fastcall RtsComponentType8_Constructor(void *this_ptr, void *edx_unused, int index, char arg0)` | confidence: high | well-structured constructor matching caller description and standard object initialization patterns. | source: llm_

**Initializes a newly allocated component object of type 8, setting up its virtual method table, configuration index, and lookup-derived flags.**

The function acts as a constructor for a specific object type (component index 8). It receives the object pointer in `eax`, an integer index in `ecx`, and an argument on the stack at `arg_0`. It populates various fields of the object structure, including setting the virtual method table pointer twice (initially to `off_41D464` and then finalized as `off_41D59C`), storing the `edx` register value into offset `+8`, and storing byte-sized configurations derived from `ecx` and `arg_0`. It clamps or masks the index parameter `ecx` against a limit of 18h (24), computes an offset into a lookup table (`byte_41BDC0`), extracts a bitflag from that table entry (`[edx+1]` shifted and masked), and stores it into the object structure.

**Parameters:**
- `this_ptr` (void*, eax) - Pointer to the newly allocated component memory block.
- `index` (int, ecx) - Index or type identifier passed in ecx.
- `arg_0` (char, esp+4) - Additional constructor byte argument passed on the stack.

**Returns:** none

**Side effects:**
- Initializes fields of the object pointed to by eax
- Sets virtual method table pointers

**Data structures:**
- ComponentObject: +0x0 vtable ptr, +0x4 int field, +0x8 pointer/data, +0xC lookup table entry ptr, +0x10 byte index, +0x11 byte arg, +0x12 extracted flag byte, +0x13 byte zero

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411691  
  Called as the constructor/initializer for factory case 8, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponent7_Constructor - `0x004121B0`

_original name `sub_4121B0` | `void* __fastcall RtsComponent7_Constructor(void *this, int edx, int ecx_id, char arg_0)` | confidence: high | Disassembly is fully clear, matches caller context regarding factory case 7 initialization. | source: llm_

**Initializes a newly allocated component object (case 7 in the factory dispatcher) using an ID index, a configuration byte argument, and a lookup table in read-only memory.**

The function acts as a constructor or initializer for a specific sub-component type created by the factory dispatcher. It receives the object pointer in `eax`, a pointer/parameter in `edx`, an ID index in `ecx`, and an argument on the stack (`arg_0`). It sets up virtual table pointers at offset 0 (overwriting `off_41D464` with `off_41D568`), stores `edx` at offset `+8`, and performs clamping/clipping logic on `ecx` using conditional instructions (`cmp ecx, 18h`, `setnl`) and bitwise operations to compute an index into a global configuration array at `byte_41BDC0`. It then extracts a bitfield from the retrieved table entry by shifting and masking, storing various flags and bytes into fields at offsets `+10h`, `+11h`, `+12h`, and clearing offset `+13h`. Floating-point instructions (`fldz`, `fst`, `fstp`) are executed during the setup, likely as part of zeroing or initializing float fields or maintaining stack hygiene.

**Parameters:**
- `this` (void*, eax) - Pointer to the newly allocated component memory block.
- `edx_param` (void*, edx) - Auxiliary pointer or value stored into [eax+8].
- `ecx_id` (int, ecx) - Index or identifier used for lookup and range clipping.
- `arg_0` (char, [esp+4]) - Configuration byte stored at [eax+11h].

**Returns:** none

**Side effects:**
- Initializes the fields and virtual table of the object instance pointed to by eax
- Reads from global configuration table byte_41BDC0

**Data structures:**
- ComponentObject: +0x0 vptr (set to off_41D568), +0x4 dword (set to 0), +0x8 dword (edx_param), +0x0C dword (pointer into byte_41BDC0), +0x10 byte (ecx id/clamped byte), +0x11 byte (arg_0), +0x12 byte (extracted flag from table entry), +0x13 byte (set to 0)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411674  
  Called as the constructor/initializer for factory case 7, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponentType6_Init - `0x00411DD0`

_original name `sub_411DD0` | `void __fastcall RtsComponentType6_Init(void *obj, int unused, int arg1)` | confidence: high | standard object constructor initialization sequence based on caller context | source: llm_

**Initializes a subtype 6 component object instance using a lookup table (`byte_41BDC0`) and index parameters.**

The function initializes a newly allocated component structure passed in `eax`. It sets up a virtual table pointer (`off_41D464`, later overwritten with `off_41D534`), stores context pointers and fields like an ID/index value from `ecx` at offset +0x10 and a byte from stack argument 0 at offset +0x11. It performs a boundary/clamping calculation on the index `ecx` against the threshold 0x18, uses the result to index into a lookup table (`byte_41BDC0`), extracts and transforms a bitflag from the lookup table entry to store at offset +0x12, and zeroes out other fields.

**Parameters:**
- `obj` (void *, eax) - Pointer to the newly allocated component memory block being initialized.
- `index` (int, ecx) - Index or type identifier used to compute lookup table offsets.
- `arg_0` (unsigned char, [esp+4]) - An additional configuration byte parameter passed from the caller.

**Returns:** none

**Side effects:**
- Initializes fields of the component structure at `eax`

**Data structures:**
- ComponentType6: +0x0 vtable ptr, +0x4 dword, +0x8 ptr/data, +0xC lookup ptr, +0x10 byte index, +0x11 byte arg_0, +0x12 byte flags, +0x13 byte

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411657  
  Called as the constructor/initializer for factory case 6, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### FactoryCase5Constructor - `0x00411CB0`

_original name `sub_411CB0` | `void* __fastcall FactoryCase5Constructor(void *this, int edx_unused, int ecx_val, char arg0)` | confidence: high | disassembly is straightforward and aligns precisely with caller description | source: llm_

**Initializes a sub-component object (case 5 in the factory dispatcher) by setting up a virtual table, storing configuration indexes and pointers, and initializing internal state fields from a lookup table.**

The function acts as a constructor/initializer for a specific factory-created object instance (passed in `eax`). It sets up the object's virtual table pointer (`off_41D500`), stores the pointer provided in `edx` at offset +8, and copies various index/argument bytes from `ecx` and stack argument `arg_0` into specific member offsets (+0x10, +0x11). It performs an arithmetic lookup involving a threshold check against `ecx` (24 / 0x18), uses the result to index into a static data table (`byte_41BDC0`), and saves a processed bitfield extracted from that table entry into offset +0x12.

**Parameters:**
- `this` (void*, eax) - Pointer to the newly allocated object instance being initialized
- `edx_arg` (void*, edx) - A pointer argument stored at offset +8 of the object
- `ecx_val` (int, ecx) - Index or identifier value used for configuration and table lookup
- `arg0` (char, esp+4) - An additional byte parameter stored at offset +0x11

**Returns:** none

**Side effects:**
- Initializes the memory block pointed to by eax as a C++ object instance, writing a vtable pointer and configuring internal fields

**Data structures:**
- Object: +0x0 vtable ptr, +0x4 int (0), +0x8 void*, +0xC const void*, +0x10 char, +0x11 char, +0x12 char, +0x13 char

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0041163A  
  Called as the constructor/initializer for factory case 5, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponentCase4_Init - `0x004119C0`

_original name `sub_4119C0` | `void __fastcall RtsComponentCase4_Init(void *this, void *edx_unused, int index, char arg1)` | confidence: high | based on clear register usage, structural member initialization, and caller context. | source: llm_

**Initializes a specific RTS component object (case 4 in the factory dispatcher) using an index, a byte parameter, and a configuration table look-up.**

The function initializes an object instance whose pointer is passed in `eax` (acting as `this`). It sets up virtual method table pointers at offset 0, stores arguments and calculated indices across various offsets of the structure, performs bounds-checking and clamping on the `index` parameter (`ecx`) against the value 24, and indexes into a lookup table (`byte_41BDC0`) using the adjusted index value. It also extracts a bit flag from a byte in the looked-up table entry, stores it at offset 0x12, and cleans up the x87 floating-point stack after a `fldz` operation.

**Parameters:**
- `this` (void*, eax) - Pointer to the newly allocated component object being initialized.
- `index` (int, ecx) - Type or index identifier passed from the factory dispatcher.
- `arg1` (char, esp+4) - Additional configuration byte passed on the stack.

**Returns:** none

**Side effects:**
- Initializes fields of the object at `eax`
- Reads from global lookup table byte_41BDC0

**Data structures:**
- ComponentObject: +0x0 vtable pointer, +0x4 padding/field, +0x8 pointer/field, +0xC table pointer, +0x10 index byte, +0x11 config byte, +0x12 extracted flag byte, +0x13 zero-initialized byte

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x0041161D  
  Called as the constructor/initializer for factory case 4, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### RtsComponentType3_Constructor - `0x004117B0`

_original name `sub_4117B0` | `void* __fastcall RtsComponentType3_Constructor(void *this, void *edx, int arg_ecx, char arg_0)` | confidence: high, clear assembly mapping to a C++ object constructor pattern with field initializations and vtable assignment. | source: llm_

**Initializes a newly allocated component object (factory case 3) by setting up its virtual table, storing configuration parameters, clamping and scaling an index to reference a lookup table entry, and extracting flags.**

The function acts as a constructor or initializer for a specific object instance (likely representing a game component or entity in the RTS engine). It receives the object pointer in `eax`, a parameter in `edx`, an integer index/ID in `ecx`, and an argument on the stack (`arg_0`). It sets up an initial virtual table pointer at offset 0 (`off_41D464`), clears offset +4, and stores `edx` at offset +8. It performs a conditional check on `ecx` (`cmp ecx, 18h`), clamping or wrapping the index value if it is greater than or equal to 24. It then multiplies this clamped index by an element size to compute an offset into a global configuration lookup table (`byte_41BDC0`), storing a pointer to the resulting table entry at offset +12. Additional configuration flags and bytes are extracted from the lookup table entry and the input parameters, stored at offsets +16 through +19, and finally the virtual table pointer is updated to its final value (`off_41D498`).

**Parameters:**
- `this` (void*, eax) - Pointer to the newly allocated component memory block being initialized.
- `param_edx` (void*, edx) - A pointer or reference value stored into offset +8 of the object.
- `param_ecx` (int, ecx) - An index or type identifier used to index into the lookup table byte_41BDC0.
- `arg_0` (char, [esp+4]) - An additional byte parameter stored at offset +17 of the object.

**Returns:** none

**Side effects:**
- Initializes fields of the object pointed to by eax
- References global read-only data structures off_41D464, off_41D498, and byte_41BDC0

**Data structures:**
- Component Object: +0x0 vtable pointer, +0x4 dword (0), +0x8 pointer, +0xC table entry pointer, +0x10 byte (clamped index/param), +0x11 byte (arg_0), +0x12 byte (extracted flag from table), +0x13 byte (0)

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x00411600  
  Called as the constructor/initializer for factory case 3, receiving the newly allocated memory in `edx`, index `esi` in `ecx`, and `edi` pushed on the stack.

### CComponentType2Factory_Constructor - `0x00414150`

_original name `sub_414150` | `void* __fastcall sub_414150(void *this_ptr, void *edx_unused, int arg_ecx, int arg_0)` | confidence: high due to clear structural initialization patterns, field offsets, and table indexing logic matching factory constructor idioms. | source: llm_

**Initializes a specific component object (case 2 factory type) using configuration tables and a provided parameter index.**

The function initializes a memory block passed in `eax` (acting as `this` pointer). It sets up virtual function table pointers (`off_41D464` then overwritten with `off_41D7A4`), stores the passed `edx` pointer at offset `+8`, and configures fields using values derived from `ecx` (clamped or masked against `18h`) and a byte array located at `0x0041BDC0`. It computes an indexed offset into the table `byte_41BDC0` using multiplication, extracts bit-level configuration flags from the table entry, stores them at specific offsets within the object structure, and clears or initializes remaining fields.

**Parameters:**
- `this_ptr` (void*, eax) - Pointer to the newly allocated component memory block.
- `type_index` (int, ecx) - Type or index identifier used for configuration and table lookup.
- `extra_arg` (unsigned char, [esp+4]) - Additional context parameter stored into the object structure.

**Returns:** none

**Side effects:**
- Initializes the fields and virtual method table of the object instance pointed to by eax.

**Data structures:**
- ComponentObject: +0x0 vtable pointer, +0x4 unknown/zero, +0x8 pointer, +0xC table entry pointer, +0x10 cl field, +0x11 extra_arg field, +0x12 extracted flag byte, +0x13 byte zero, +0x1C zero field

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004115E3  
  Called as the constructor/initializer for factory case 2, receiving the newly allocated memory in `edx`, the index `esi` in `ecx`, and additional context (`edi`) pushed on the stack.

### RtsSubComponentA_Init - `0x00413870`

_original name `sub_413870` | `void* __stdcall RtsSubComponentA_Init(void *obj, int arg0, int arg1)` | confidence: high | standard object constructor pattern with register and stack parameter unpacking | source: llm_

**Initializes a specific sub-component object instance (case A of the factory dispatcher) using configuration parameters and a lookup table.**

The function initializes an object instance passed in `eax`. It populates various fields of the object structure, setting up virtual method table pointers (vftable at `off_41D464` overwritten later by `off_41D770`), storing incoming parameters into byte/word fields, performing a clamped calculation or array index lookup into `byte_41BDC0` based on `arg_0`, zeroing out multiple subsequent fields, and setting up internal state flags based on the lookup table and arguments.

**Parameters:**
- `obj` (void*, eax) - Pointer to the newly allocated object memory block being initialized.
- `arg_0` (int, esp+4) - Primary configuration integer / type index.
- `arg_1` (int, esp+8) - Secondary configuration parameter (lower byte copied to offset 0x11).

**Returns:** none

**Side effects:**
- Initializes the fields of the object pointed to by eax
- References global configuration/lookup tables byte_41BDC0 and virtual tables off_41D464 / off_41D770

**Data structures:**
- Object struct: +0x0 vftable, +0x4 pointer/data, +0x8 stored edx, +0x0C lookup pointer, +0x10 byte arg0, +0x11 byte arg1, +0x12 byte flag, +0x13-0x1F zeroed fields, +0x20 dword zero, +0x24 dword zero

**Called by:**
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004115C6  
  Called as the constructor/initializer for the first factory case (type index mapped via switch), receiving the newly allocated memory block in `edx`, `esi` (or `edi` via push), and setting up the object.

### Vec3_NormalizeOrZero - `0x00417FD0`

_original name `sub_417FD0` | `float __cdecl Vec3_NormalizeOrZero(float x, float y, float z)` | confidence: high, standard vector normalization and safe-inverse-sqrt pattern in x87 assembly | source: llm_

**Computes the Euclidean length of a 3D vector and normalizes it, returning 1/length or 0 if the length is zero or near-zero.**

The function receives 3 floating-point coordinates (likely on the x87 FPU stack or passed as arguments). It computes the sum of squares ($x^2 + y^2 + z^2$), takes the square root using _CIsqrt to find the magnitude, and compares it against zero. If the magnitude is valid (non-zero), it computes the reciprocal ($1 / \text{length}$) and scales each component of the vector by this reciprocal before returning. If the vector length is zero or near-zero, it avoids division by zero and returns appropriate zero/one defaults.

**Parameters:**
- `x` (float, st(2) / stack) - X component of the vector
- `y` (float, st(1) / stack) - Y component of the vector
- `z` (float, st(0) / stack) - Z component of the vector

**Returns:** float in st(0), or vector components modified in-place/returned via FPU stack

**Called by:**
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x004067CB  
  Called at 0x004067CB to perform a spatial or mathematical calculation, consuming floating-point context values.
- `EvaluateEntityAbilityCriteriaAndCast` (0x004071C0) - call at 0x004078A9  
  Called during candidate validation or filtering to compute geometric or spatial relationships (such as vector differences or projections) for targeting criteria.
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x0040577D  
  Called after vector subtraction and initialization (at 0x0040577D) to compute a geometric operation or transform on difference vector components before normalization and grid/index mapping.
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x004020C9  
  Called early in the function to initialize or perform setup calculations using floating-point values.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00417FEE

### memcpy - `0x0041A22A`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!memcpy.**

A single `jmp ds:__imp_memcpy` through the import address table entry for MSVCR80!memcpy - calling this function is exactly calling memcpy.

**Called by:**
- `VerifyResourceBlocks` (0x00409280) - call at 0x00409334
- `ResourceBuffer_FlushOrWritePacket` (0x00408B60) - call at 0x00408BB4
- `WriteBufferPacket` (0x00408FE0) - call at 0x00409089
- `AppendToDynamicBuffer` (0x00408F70) - call at 0x00408FCA
- `ResizeOrCompactContainer` (0x00403520) - call at 0x0040362D
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x0041943B
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401B99, 0x00401E88
- `ReallocateCustomBlock` (0x00418670) - call at 0x004186BE

### ResourceEntry_Initialize - `0x00410A90`

_original name `sub_410A90` | `void* __fastcall ResourceEntry_Initialize(void *this, void *edx, int Src)` | confidence: high, based on clean structure initialization pattern, standard MSVC optimization idioms, and caller context. | source: llm_

**Initializes a newly allocated resource or sub-system entry object passed in `ecx` using an identifier/lookup value (`Src`) passed in `eax`.**

The function takes a pointer to an object/structure in `ecx` (acting as a `this` pointer) and a source identifier value in `eax`. It stores a virtual table pointer at offset +0, clears or initializes various status fields and flags within the structure (such as writing specific bytes/words at offsets +4, +8, +0xC, +0x18, +0x1A, +0x1B, +0x1C, +0x1E, +0x20), and performs arithmetic/floating-point operations using constants from `.rdata`. Notably, it computes a division/scaling calculation using a reciprocal multiplication constant (`0x2AAAAAABh`, effectively dividing a value by 3 or mapping coordinates/indices), stores intermediate values, and returns the pointer `ecx` in `eax`.

**Parameters:**
- `this` (void*, ecx) - Pointer to the resource entry structure being initialized.
- `Src` (int, eax) - Lookup value or identifier used to compute indices/fields within the structure.

**Returns:** Returns the initialized structure pointer in eax.

**Side effects:**
- Initializes fields of the object pointed to by ecx

**Data structures:**
- ResourceEntry: +0x0 vtable/type ptr, +0x4 int/flags, +0x8 int/offset, +0xC int/value, +0x18 word flags, +0x1A byte field, +0x1B byte field, +0x1C word/short id, +0x1E word field, +0x20 byte field

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401A86  
  Called with a lookup/identifier value (`Src`) to create or fetch a resource entry, returning a pointer in `eax` that is subsequently inserted into a tracking table.

### UpdateResourceStateBlockOrSimulationStep - `0x00416640`

_original name `sub_416640` | `__stdcall void UpdateResourceStateBlockOrSimulationStep(void *resourceBlock, void *dataBuffer, int count)` | confidence: high | complete disassembly mapped to sub-systems and verified through caller context | source: llm_

**Performs a comprehensive simulation, procedural generation, spatial grid update, and entity/particle batch processing step using a large state structure.**

The function begins by updating an internal state marker and mixing seed/resource state values at the start of the resource block. It then calls sub_415220 [GenerateNoiseHeightmap] to generate procedural noise data, followed by multiple invocations of sub_4152B0 [ProcessSpatialGridOrCollisionData] and sub_4153F0 [ComputeDistanceFieldOrGridCollision] using constant rdata floating-point factors. It proceeds to update particle systems or emitters via sub_415D40 [UpdateParticleSystemOrEmitter], updates particle batches and grid cell states using sub_4156B0 and sub_415770, packs local stack coordinate arrays, and executes sub_4159E0 [ProcessSpatialGridBatchOrParticleUpdate]. Finally, it loops through a collection of items, converting floating-point coordinates via __ftol2_sse, validating spatial positions using sub_414E40 [CheckGridAreaOrFindNearby], and updating coordinate entries within the resource block.

**Parameters:**
- `resourceBlock` (void *, [esp+arg_0] (accessed via register context in caller as edi)) - Pointer to the large resource/state block structure being updated.
- `dataBuffer` (void *, [esp+arg_4]) - Pointer to a secondary data or coordinate buffer used across spatial and collision subroutines.
- `count` (int, [esp+arg_8]) - Element count or iteration bound derived from resource state.

**Returns:** none

**Side effects:**
- Modifies fields within the resource/state block passed in esi
- Updates particle and spatial grid state data
- Performs extensive x87 floating-point calculations and conversions

**Data structures:**
- Resource block structure: contains fields at +0x0, +0x4, +0xE6C0 (array buffer), +0xE6E0 (count), +0xE6E4 (coordinate/metric array), +0xE744 (loop count), +0xE748 (status flag)

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401838  
  Called to process or initialize a block of data, passing a target structure pointer (edi), a local stack buffer (`var_20`), and an element count derived from resource state (`edx`).

**Calls:**
- `GenerateNoiseHeightmap` (0x00415220) - call at 0x0041668D
- `ProcessSpatialGridOrCollisionData` (0x004152B0) - call at 0x004166C8, 0x004166E7, 0x00416773
- `ComputeDistanceFieldOrGridCollision` (0x004153F0) - call at 0x0041670E, 0x00416730
- `UpdateParticleSystemOrEmitter` (0x00415D40) - call at 0x0041674A
- `UpdateParticleBatchState` (0x004156B0) - call at 0x0041678A
- `UpdateGridCellStates` (0x00415770) - call at 0x00416791
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00416807
- `__ftol2_sse` (0x0041A160) - call at 0x00416845, 0x00416889
- `CheckGridAreaOrFindNearby` (0x00414E40) - call at 0x004168CB

### CheckGridAreaOrFindNearby - `0x00414E40`

_original name `sub_414E40` | `bool __stdcall CheckGridAreaOrFindNearby(void *gridBase, int *coord1, int *coord2, int maxRadius)` | confidence: high | source: llm_

**Validates a 2D grid cell or searches outward in concentric square rings up to a given radius for a valid/accessible grid position, returning the first matching coordinate via output pointers.**

The function begins by checking if the initial coordinates (passed via pointers `arg_4` and `arg_8`) fall within valid 0-to-96 grid bounds (0x60). If valid, it indexes into a grid state array at `gridBase + 0xB704` using the formula `index = (row * 3 + col) * 32 + col` (or similar stride calculation) and tests a bit flag. If `maxRadius` (arg_C) is less than 1, it simply returns whether that initial cell satisfies the condition. Otherwise, it loops outward in square shells ('rings') of increasing radius up to `maxRadius`. For each shell, it checks a series of offset positions. If a position passes the grid validation check (bit test on the grid byte), the function writes the matched coordinates back to the input pointers `arg_4` and `arg_8` and returns true (1). If no matching position is found across all rings, it returns false (0).

**Parameters:**
- `gridBase` (void*, [esp+34h+arg_0]) - Base pointer to the grid state structure containing the lookup table at offset +0xB704.
- `coordX` (int*, [esp+34h+arg_4]) - Pointer to the X coordinate (column), updated with the found position on success.
- `coordY` (int*, [esp+34h+arg_8]) - Pointer to the Y coordinate (row), updated with the found position on success.
- `maxRadius` (int, [esp+34h+arg_C]) - Maximum search radius (number of rings to check outward).

**Returns:** bool (1 for success/valid, 0 for failure/not found) in al

**Side effects:**
- Modifies the values pointed to by coordX and coordY if a nearby valid cell is found during ring expansion.

**Called by:**
- `ExecuteSpawnEntityScriptOpcode` (0x0040A3F0) - call at 0x0040A4BC  
  Called with a pointer to a sub-structure within global state (at esi+1A20h), a pointer to coordinates, and size 8, to query or validate a region or transform, returning a boolean result in al that conditionally gates subsequent math operations.
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x0040245C  
  Called inside an outer nested loop over 5 iterations to check or update spatial resource status, passing pointer structures (including var_68 and var_60) and offset 0x1A20 of the resource block.
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x004168CB  
  Called inside an iterative loop over resource parameters, passing `esi`, clamped integer coordinate arguments, and a constant (0Ah) to validate or query grid/table data, using the boolean return value (`al`) to conditionally update values in a structure (`ebx`).

### ProcessSpatialGridBatchOrParticleUpdate - `0x004159E0`

_original name `sub_4159E0` | `void __stdcall ProcessSpatialGridBatchOrParticleUpdate(void *pRngState, float arg_4, int arg_8)` | confidence: high, based on clear loop bounds matching grid sizes (93x93 = 5Dh x 5Dh), identifiable math imports like _CIsqrt, and robust helper function calls. | source: llm_

**Iterates through a nested 2D grid/cell structure to perform geometric metric calculations, evaluate procedural noise, check thresholds, and update state records.**

The function begins by performing a mix-and-multiply pseudo-random number generator step using the state structure passed via arg_0 (esi). It then enters a nested loop structure iterating through grid coordinates (up to 93x93 dimensions), checking flags within a structure based at ebx + 0xB824. For valid cells, it performs iterative calculations involving square roots (_CIsqrt), spatial metrics (CalculateSpatialOrPathMetric), and parameter clamping (ClampFloat). It queries procedural noise using EvaluateProceduralNoiseGrid and combines results with conditional floating-point logic, thresholds, and pseudo-random state updates via UpdateRngAndCompute. If conditions are met and a counter (ebx + 0xE6BCh) is below 5DC16 (1500), it records cell indices into an output array table within the structure at ebx.

**Parameters:**
- `pRngState` (void*, arg_0 ([ebp+0x8])) - Pointer to a 64-bit random number generator or hash state structure.
- `arg_4` (float, arg_4 ([ebp+0xC])) - Floating-point parameter used in geometric distance and spatial calculations.
- `arg_8` (int, arg_8 ([ebp+0x10])) - Count or limit parameter for inner iteration loops.

**Returns:** none

**Side effects:**
- Modifies the RNG/hash state pointed to by arg_0
- Updates grid state flags and index tables located via ebx

**Data structures:**
- `RngState` (uint32_t[2]) - Pseudo-random number generator / hash state fields updated at the start

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x00416807  
  Called in a loop/processing block, passing `esi` in `ebx`, `arg_0`, a local stack buffer (`var_40`), and a count in `ecx`, to feed back or register processed local items into the main structure.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00415AE1
- `EvaluateProceduralNoiseGrid` (0x00415180) - call at 0x00415B6B
- `CalculateSpatialOrPathMetric` (0x00418050) - call at 0x00415BE7, 0x00415C29
- `ClampFloat` (0x00418140) - call at 0x00415BF2, 0x00415BFA, 0x00415C34, 0x00415C3C
- `UpdateRngAndCompute` (0x004188F0) - call at 0x00415CB3

### UpdateRngAndCompute - `0x004188F0`

_original name `sub_4188F0` | `double __cident __fastcall UpdateRngAndCompute(void *pRngState)` | confidence: high, clear PRNG state-update bit-mixing pattern combined with x87 FPU math. | source: llm_

**Updates a 64-bit pseudo-random number generator state (or hash/state structure) passed via `esi`, performs bitwise mixing and arithmetic operations, and executes a complex sequence of x87 FPU calculations returning a floating-point value.**

The function begins by reading a 64-bit value from the structure pointed to by `esi` (`[esi]` and `[esi+4]`). It performs a series of bitwise operations, multiplications, and XOR shifts combined with the constant `0x3C6EF35F` to update the state, writing the updated state back to `esi`. It then converts the resulting value into an FPU register via `fild` and applies conditional adjustments based on sign, division, and a sequence of arithmetic operations on the FPU stack (involving subtractions, multiplications, and additions) to produce a final floating-point result loaded into `st(0)`.

**Parameters:**
- `pRngState` (esi) - Pointer to a 64-bit state structure (e.g., PRNG state or similar).

**Returns:** st(0) - floating-point result calculated from the updated state.

**Side effects:**
- Modifies the 64-bit state structure pointed to by esi.

**Data structures:**
- State structure: +0x00 uint32_t low, +0x04 uint32_t high

**Called by:**
- `ExecuteRandomRangeOp` (0x0040A380) - call at 0x0040A3CC  
  Called as part of evaluating or performing the operation for this bytecode opcode, operating on floating-point values prepared on the x87 FPU stack and utilizing a global context/runtime structure referenced via 0x0042AC60.
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00415CB3  
  Called near the end of the validation block when specific comparison thresholds are met, passing parameters via the stack and FPU registers to register or finalize a state update.

### ClampFloat - `0x00418140`

_original name `sub_418140` | `float __cdecl ClampFloat(float value, float min_val, float max_val)` | confidence: medium | standard x87 floating-point clamp/min-max routine pattern based on register comparisons and conditional jumps | source: llm_

**Performs a clamp operation on a floating-point value between a lower and upper bound using x87 FPU instructions.**

The function implements a standard clamping operation on the x87 floating-point stack. It takes a value and two boundary limits, comparing them using fcom and testing status flags via fnstsw to restrict the input value to the specified [min, max] range before returning the result in st(0).

**Parameters:**
- `value` (float, st(0) on entry) - The input floating-point value to be clamped.
- `min_val` (float, st(1) on entry (pushed)) - The lower bound limit.
- `max_val` (float, st(2) on entry (pushed)) - The upper bound limit.

**Returns:** st(0) containing the clamped floating-point value

**Called by:**
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x00402106  
  Called near the beginning of the processing sequence after sub_418050 to perform a sub-step in the configuration or coordinate computation.
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00415BF2, 0x00415BFA, 0x00415C34, 0x00415C3C  
  Called repeatedly in succession alongside sub_418050 to process dependent floating-point parameters or perform follow-up calculations on the active item.

### CalculateSpatialOrPathMetric - `0x00418050`

_original name `sub_418050` | `double __cdecl CalculateSpatialOrPathMetric()` | confidence: medium, because it is a pure math/geometry utility operating entirely on FPU registers without explicit memory parameters, making exact algebraic identification difficult without full context. | source: llm_

**Performs complex floating-point geometry, clamping, and polynomial evaluation calculations using values on the x87 FPU stack.**

The function operates entirely on the x87 floating-point stack, performing a series of conditional comparisons (using fcom and fnstsw / test ah, 5/41h), absolute values, clamping operations, and polynomial arithmetic involving constant values loaded from .rdata. It manages the FPU stack depth through multiple pushes, pops, and interchanges (fxch), ultimately leaving the final computed result in st(0) upon return.

**Returns:** st(0) (double-precision floating-point result)

**Called by:**
- `ProcessRtsResourceSpawningOrPathNodes` (0x00407B50) - call at 0x00407D85  
  Called once per iteration inside the main processing loop to perform a calculation or state update on the newly populated record entry (using data derived from the current item index and coordinate tables).
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x004020FB, 0x0040247D  
  Called at multiple points (e.g., around 0x004020FB and 0x0040247D) to perform floating-point utility calculations or math transformations during resource processing.
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00415BE7, 0x00415C29  
  Called at multiple points during the processing block, passing floating-point values and local/structure arguments on the stack/FPU to perform a specialized mathematical transformation or resource query.

### UpdateGridCellStates - `0x00415770`

_original name `sub_415770` | `void __cdecl UpdateGridCellStates(void *gridData)` | confidence: high | standard grid iteration pattern with nested loops, constant floating-point loads, and neighbor bitmask updates. | source: llm_

**Iterates over a 2D grid of elements (from index 2 to 93/0x5E), performing neighbor comparisons, distance/geometric checks, and updating cell state flags.**

The function performs a nested loop over a 2D coordinate space ranging from 2 up to 96 (0x60, exclusive bound 0x5E for inner processing). For each cell, it loads structural data offset by `edi` and evaluates neighboring cells in four cardinal directions (plus/minus offsets in X and Y). It uses floating-point calculations loaded from .rdata constants to evaluate spatial thresholds or distances, tests condition flags via FPU status words (fnstsw), and sets bit flags (such as OR-ing bits 2, 3, or 0x23) directly into status bytes located within the grid structure at offsets 0x9300–0xB704.

**Parameters:**
- `gridData` (void *, edi) - Pointer to a large grid or state structure containing bytes and coordinate fields processed in batches.

**Returns:** none

**Side effects:**
- Modifies bytes within the grid data structure at offsets around +0x9300 to +0xB704, specifically setting flag bits.

**Data structures:**
- Grid structure: 2D array or grid spanning at least 0x60 x 0x60 elements, with neighbor/state byte tables starting around offset +0x9303 and flag bytes around +0xB704.

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x00416791  
  Called immediately after sub_4156B0 with `esi` in `edi` to clean up or reset auxiliary structures.

### UpdateParticleBatchState - `0x004156B0`

_original name `sub_4156B0` | `void __fastcall UpdateParticleBatchState(void *pStateObject)` | confidence: high, clear loop structure with 96x96 bounds (0x60) and standard x87 math/conversion patterns | source: llm_

**Performs an inner loop batch update over a multi-dimensional array of particle or entity state elements, applying floating-point math, bounds clamping, and integer conversions to populate lookup flags and byte codes.**

The function processes a nested grid/array structure passed via `eax`. It executes an outer loop running 96 iterations (0x60) and an inner loop running 96 iterations per outer step (totaling 9,216 items). Within the inner loop, it loads constant floating-point values from .rdata, performs a series of arithmetic operations (`fadd`, `fmul`), and converts the result to an integer using `__ftol2_sse`. This integer value is then clamped to the inclusive range [0, 3]. Simultaneously, floating-point comparisons (`fcomp`, `fnstsw`) evaluate conditional flags (checking status word bits) to assign status bytes and control flags into destination arrays located at fixed offsets relative to the base pointers `ebx` and `edi` derived from `eax`.

**Parameters:**
- `pStateObject` (void *, eax) - Pointer to the root state or particle system structure being updated

**Returns:** none

**Side effects:**
- Modifies byte arrays within the structure pointed to by eax at offsets derived from ebx and edi

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x0041678A  
  Called with `esi` as `eax` after processing arrays/buffers to perform a synchronization, commit, or state transition step.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x004156F8

### UpdateParticleSystemOrEmitter - `0x00415D40`

_original name `sub_415D40` | `void __cdecl UpdateParticleSystemOrEmitter(void *pRng, void *pEmitterState)` | confidence: medium | source: llm_

**Updates particle generator state and buffers using extensive floating-point calculations, coordinate transformations, and pseudo-random number generation.**

The function processes a large object structure (passed in `esi`) representing a particle emitter or effect system. It performs a series of complex arithmetic and trigonometric operations involving vertex buffers or particle vectors starting at offsets 0xE6C0 and 0xE6E4 within the structure. It loops through multiple phases to generate or update particles, utilizing a PRNG struct (passed as arg_0) to introduce randomized offsets and velocities. Floating-point conversions via `__ftol2_sse` are used to discretize continuous values into integer indices or coordinate grids. The routine handles multiple internal particle list counters (such as `+0xE6E0` and `+0xE744`), adding or updating elements conditionally based on loop bounds and thresholds.

**Parameters:**
- `pRng` (void*, stack [esp+0x44 + arg_0]) - Pointer to a random number generator state structure used for particle randomization.
- `pEmitterState` (void*, stack [esp+0x44 + arg_4]) - Pointer to emitter parameters or configuration data.

**Returns:** none

**Side effects:**
- Modifies particle buffer data and count variables in the object structure pointed to by esi
- Updates the internal PRNG state structure

**Data structures:**
- `ParticleSystemState`

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x0041674A  
  Called after processing item loops, passing `ebp` in `eax`, `arg_0` and `arg_4` as arguments, to perform a batch operation or finalize a processed set of items.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x0041652D, 0x00416576

### ComputeDistanceFieldOrGridCollision - `0x004153F0`

_original name `sub_4153F0` | `float __stdcall ComputeDistanceFieldOrGridCollision(void *grid_data)` | confidence: medium, based on loop structure sizing (97 * 97 nested iterations) and heavy usage of x87 floating-point geometry/distance calculations | source: llm_

**Iterates through a multi-dimensional grid structure (up to 9417 elements in nested loops) performing geometric distance calculations and collision/intersection checks using x87 floating-point math and square roots.**

The function performs a nested loop traversal over a large grid or spatial structure. The outer loop runs up to 24C1h (9413) in steps of 61h (97), and the inner loop runs 61h (97) times, yielding a total of ~9417 iterations. Inside the loops, it manages coordinate indexing, calls `CalculateGridPointerOrMetric` to retrieve grid/metric values based on position, compares values against floating-point thresholds stored in `.rdata`, performs vector/geometric computations (dot products, coordinate differences), and computes square roots using `_CIsqrt` to find minimum distances, ray intersections, or grid collision penalties.

**Parameters:**
- `grid_data` (void *, eax) - Pointer to resource/state data structure or grid buffer passed in eax

**Returns:** st(0) (floating-point result or metric)

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x0041670E, 0x00416730  
  Called inside a loop over element counts (`ebp`), passing index/buffer pointers in `ecx` and data pointers in `eax`, to compute or transform pair-wise elements.

**Calls:**
- `CalculateGridPointerOrMetric` (0x00414AF0) - call at 0x0041542F, 0x0041543B
- `_CIsqrt` (0x0041A212) - call at 0x004155DA

### CalculateGridPointerOrMetric - `0x00414AF0`

_original name `sub_414AF0` | `float __fastcall CalculateGridPointerOrMetric(float arg_0)` | confidence: medium, heavy x87 floating-point arithmetic makes exact symbolic reconstruction of expressions complex, but overall structure (clamping -> integer conversion -> grid indexing -> distance metric calculation) is clear. | source: llm_

**Performs geometric bounding/clipping checks and coordinate-to-grid index calculations using x87 floating-point math, returning a computed pointer or metric value.**

The function begins by manipulating several floating-point values on the x87 FPU stack, involving constants from .rdata (0x0041D960 and 0x0041D970) and performing conditional min/max or clamping logic via FPU comparison sequences (fcom, fnstsw, test ah, 5). If a certain bounding check fails, it branches to an alternate path or cleans up the FPU stack. Otherwise, it converts parts of the evaluated floating-point coordinates into integers using __ftol2_sse, computes a linear grid offset using multiplication and addition (imul), and offsets a base pointer passed via arguments. The remainder of the function continues complex vector/distance or interpolation math using dot-product-like or squared-distance accumulation patterns on the FPU stack, ultimately returning a floating-point result in st(0).

**Parameters:**
- `base_ptr_or_struct` (void*, [esp+10h+arg_0]) - Base pointer or structure reference used for calculating final indexed memory locations.

**Returns:** st(0) (floating-point result)

**Called by:**
- `ComputeDistanceFieldOrGridCollision` (0x004153F0) - call at 0x0041542F, 0x0041543B  
  Called twice in the initialization phase of the function (once with the data pointer in `esi` and once via other register state) to retrieve or transform configuration/resource parameters before entering the nested calculation loops.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00414B63, 0x00414B74

### ProcessSpatialGridOrCollisionData - `0x004152B0`

_original name `sub_4152B0` | `void __cdecl ProcessSpatialGridOrCollisionData(void *arg_0)` | confidence: medium, heavy floating-point and x87 FPU usage makes exact variable semantics obscure, but loop bounds (97 x 97 grid-like indices) and math/sqrt point strongly toward spatial/collision calculations. | source: llm_

**Performs intensive nested-loop floating-point calculations, coordinate distance/intersection evaluations, and square root operations over a large grid or multi-element structured buffer (9732 iterations total across two loops).**

The function executes a nested loop structure processing geometric, spatial, or collision-related data. The outer loop runs up to 0x24C1 (9409) with a step of 0x61 (97), while the inner loop runs 0x61 (97) times per outer iteration, yielding a total of 9409 * 97 = 912,673 potential evaluation points (or rather, the outer loop advances by 97 up to 9409, and the inner loop counts up to 97, suggesting a 2D grid or indexing structure of size ~97x97 or chunks thereof). Within the loops, it loads various constants from .rdata, performs floating-point arithmetic (multiplication, subtraction, addition, division), computes square roots using _CIsqrt, and performs conditional branching based on FPU status flags (e.g., via fnstsw / test ah) to determine whether to perform further geometric corrections or distance-based calculations.

**Parameters:**
- `arg_0` (void *, [esp+34h]) - Pointer to a data structure or buffer processed by the nested loops.

**Returns:** none

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x004166C8, 0x004166E7, 0x00416773  
  Called multiple times in different loops (using `ebx = esi` and various `eax` buffer or field pointers, e.g., from `arg_4` or offset `0xE6C0`) to process or update individual elements or vector entries.

**Calls:**
- `_CIsqrt` (0x0041A212) - call at 0x00415331

### _CIsqrt - `0x0041A212`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!_CIsqrt.**

A single `jmp ds:__imp__CIsqrt` through the import address table entry for MSVCR80!_CIsqrt - calling this function is exactly calling _CIsqrt.

**Called by:**
- `UpdateEntitySlotProximityOrReferences` (0x004082D0) - call at 0x0040838A
- `UpdateElementState` (0x00410D00) - call at 0x00410DA9
- `CUnit::UpdateAIStateOrBehavior` (0x00406760) - call at 0x004069D0, 0x00406BD2
- `CEntity::FindBestTargetOrNode` (0x00408610) - call at 0x00408745
- `FindNearestValidEntityOrTarget` (0x00406540) - call at 0x004066F9
- `Entity::ProcessCombatOrAbilityUpdate` (0x00404AD0) - call at 0x00404BA1
- `Entity::FindTargetOrValidSubEntity` (0x004053D0) - call at 0x00405458
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x00405674, 0x004056CE, 0x00405955, 0x004059B5
- `CalculateDistance` (0x00417F80) - call at 0x00417FB1
- `ProcessRtsResourceSpawningOrPathNodes` (0x00407B50) - call at 0x00407C23, 0x00407C8A
- `Vec3_NormalizeOrZero` (0x00417FD0) - call at 0x00417FEE
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00415AE1
- `ComputeDistanceFieldOrGridCollision` (0x004153F0) - call at 0x004155DA
- `ProcessSpatialGridOrCollisionData` (0x004152B0) - call at 0x00415331

### GenerateNoiseHeightmap - `0x00415220`

_original name `sub_415220` | `__fastcall void GenerateNoiseHeightmap(void *targetData)` | confidence: medium, based on the double loop structure up to 0x61 (97 iterations) and calls to EvaluateProceduralNoiseGrid suggestive of heightmap or noise texture generation. | source: llm_

**Generates a 2D grid/map of procedural noise values by evaluating noise grid functions across a double loop.**

The function performs a nested loop running from 0 to 96 (0x61) in both outer and inner dimensions. Within the loops, it manipulates floating-point values using constants from the read-only data section and invokes sub_415180 [EvaluateProceduralNoiseGrid] to compute noise values for grid cells, writing or updating results into an array or buffer structure passed via the register eax (using a __fastcall convention).

**Parameters:**
- `targetData` (void *, eax) - Pointer to a resource, state, or buffer structure updated during grid generation.

**Returns:** none

**Side effects:**
- Modifies memory buffers associated with the pointer passed in eax

**Called by:**
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x0041668D  
  Called once at the beginning of the function (with `esi` as `eax`) to initialize or reset a sub-component or state associated with the main resource structure.

**Calls:**
- `EvaluateProceduralNoiseGrid` (0x00415180) - call at 0x0041526B

### EvaluateProceduralNoiseGrid - `0x00415180`

_original name `sub_415180` | `__fastcall double EvaluateProceduralNoiseGrid(int count, void *data)` | confidence: high | the function logic, loop count check, x87 FPU manipulations, and call to ComputeNoiseOrRandomCellValues are clearly identifiable. | source: llm_

**Iterates a specified number of times, accumulating values computed by a procedural noise/hash function (`ComputeNoiseOrRandomCellValues`) combined with floating-point weightings, and returns a normalized result.**

The function uses a custom fastcall-like convention where `eax` passes an integer count and `ecx` passes a pointer to a data structure. It initializes floating-point state on the x87 FPU stack (zero and one), loops `count` times if `count > 0`, and within the loop advances a pointer offset (`esi + 1Fh`), calls `ComputeNoiseOrRandomCellValues`, and performs intensive x87 floating-point arithmetic (accumulating, scaling, multiplying, and adding constants). After the loop completes, it divides or normalizes the accumulated floating-point value and returns the final result in `st(0)`. It cleans up 8 bytes of stack arguments upon return (`retn 8`).

**Parameters:**
- `count` (int, eax) - The number of iterations to perform in the evaluation loop.
- `dataPtr` (void*, ecx) - A pointer to a structure or buffer processed during the noise evaluation loop.

**Returns:** st(0) (double-precision floating-point value)

**Called by:**
- `ProcessSpatialGridBatchOrParticleUpdate` (0x004159E0) - call at 0x00415B6B  
  Called once within the inner loop structure after various floating-point operations and square-root calculations to evaluate a condition or obtain a value used in subsequent threshold and comparison checks.
- `GenerateNoiseHeightmap` (0x00415220) - call at 0x0041526B  
  Called inside the inner loop of the nested iteration (running up to 0x61 times for both outer and inner loops), likely to perform a per-element coordinate or value calculation, updating state data before subsequent floating-point arithmetic.

**Calls:**
- `ComputeNoiseOrRandomCellValues` (0x00414FA0) - call at 0x004151B4

### ComputeNoiseOrRandomCellValues - `0x00414FA0`

_original name `sub_414FA0` | `double __cdecl ComputeNoiseOrRandomCellValues(double val1, double val2)` | confidence: high, standard pattern for procedural hash/noise generation combined with math transformations matching upstream callers. | source: llm_

**Performs complex floating-point calculations combined with a pseudo-random integer mixing hash function (resembling a value noise or hash-based procedural generation routine).**

The function receives floating-point input arguments on the x87 FPU stack. It converts parts of the floating-point values to integers using __ftol2_sse, performs integer multiplications, and applies a multi-step integer mixing/hashing algorithm using bitwise XOR, shifts, and specific magic constants (such as 0x165667B1, 0x27D4EB2F, and 0x3E2B52E0). These mixed integer states are then converted back to floating-point values to participate in a series of normalization, scaling, interpolation, and arithmetic combining steps across the FPU stack. The function ultimately returns a single precision floating-point result in st(0).

**Parameters:**
- `val1` (double, x87 FPU stack (st(0) / st(1) on entry)) - First input floating-point coordinate or parameter.
- `val2` (double, x87 FPU stack (st(0) on entry)) - Second input floating-point coordinate or parameter.

**Returns:** st(0) (floating-point result)

**Called by:**
- `EvaluateProceduralNoiseGrid` (0x00415180) - call at 0x004151B4  
  Called once per iteration inside a loop running 'edi' times (where 'edi' is passed in eax), taking part in the floating-point calculations using data derived from the structure pointer in esi.

**Calls:**
- `__ftol2_sse` (0x0041A160) - call at 0x00414FAD, 0x00414FD2

### __ftol2_sse - `0x0041A160`

_`int __cdecl __ftol2_sse(void)` | confidence: high; standard MSVC runtime helper function __ftol2_sse with well-known behavior and symbol. | source: llm_

**Converts the top-of-stack x87 floating-point value to a 32-bit signed integer using SSE or fallback FPU routines, returning the integer result in EAX.**

The function implements the Microsoft Visual C++ runtime helper `__ftol2_sse`. It checks a global configuration flag at 0x00435CC8 to determine if SSE2 instructions are supported and enabled. If so, it uses an aligned stack frame, loads/converts the floating-point value from the x87 stack via `cvttsd2si` (or falls back to traditional FPU control word inspection and rounding logic if SSE2 is unavailable or FPU control word rounding modes differ). Depending on the sign, fractional parts, and QNaN/zero checks, it handles truncation and clamping to return a 32-bit signed integer in EAX.

**Returns:** eax (32-bit signed integer converted from st(0))

**Side effects:**
- pops the top value from the x87 FPU stack

**Called by:**
- `CheckEntityGridFilter` (0x00417E10) - call at 0x00417E25, 0x00417E60  
  Called twice to convert floating-point values from the x87 FPU stack into integer coordinates or indices (clamped/processed subsequently via bitwise/arithmetic checks) for accessing elements in a grid or data array.
- `CUnit::EvaluateAbilitiesAndDispatchCommands` (0x004079D0) - call at 0x00407A4E  
  Converts a floating-point calculation result (representing scaled time or range threshold) to a 32-bit integer, which is then compared against a threshold value from the entity structure.
- `CalculateGridBounds` (0x004174B0) - call at 0x004174D0, 0x00417514, 0x00417557, 0x00417599  
  Called multiple times to convert floating-point values (derived from coordinate and grid dimension calculations) into integer values. The resulting integers are then clamped to the range [0, 23] and written to output coordinate/index structures.
- `EvaluateGridLookup` (0x00417D70) - call at 0x00417D85, 0x00417DC0  
  Called twice to convert floating-point values (derived from math operations involving input coordinates and constant scaling factors) into signed 32-bit integers, which are then used as grid or coordinate indices (clamped between 0 and 95) for table/array lookup.
- `CheckEntityPlacementOrActionCriteria` (0x00405520) - call at 0x0040579A, 0x004057D9  
  Called multiple times (at 0x0040579A and 0x004057D9) to convert floating-point values from the x87 stack into integer values (via __ftol2_sse) used for clamping, index generation, and bounds checking in grid loops.
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401CA3, 0x00401CF2  
  Standard runtime helper `__ftol2_sse` called to convert floating-point values computed from table lookups into integer indices used for grid or array addressing.
- `ProcessGridRegionAction` (0x00417B50) - call at 0x00417B6F, 0x00417BBA, 0x00417BEB, 0x00417C76, 0x00417CBB  
  Called multiple times to convert floating-point coordinate or index calculations into integer values (truncating/rounding via __ftol2_sse) during grid coordinate mapping and clamping logic.
- `UpdateGridCellRegion` (0x00417A70) - call at 0x00417AD4, 0x00417AE1  
  Called to convert floating-point coordinates into integer grid indices (X and Y) within a conditional loop that processes map/grid cells.
- `VM_ExecuteBytecodeBlock` (0x00409940) - call at 0x00409F6A  
  Called to convert a floating-point value in st(0) to an integer (using SSE/ftol2), returning the integer result in EAX.
- `HandleOpcodeType2` (0x0040B0C0) - call at 0x0040B0CC  
  Called after loading a floating-point value onto the x87 FPU stack (via fld) when the inspected operand type is 2, converting the top-of-stack floating-point number into a 32-bit integer returned in EAX.
- `ExecuteVmOpcodeSlot24` (0x0040AEF0) - call at 0x0040AEFF  
  Called to convert a floating-point value (loaded via fld when the operand type tag equals 2) into an integer representation.
- `ExecuteScriptOpcode_Slot23` (0x0040AE80) - call at 0x0040AE8F  
  Called when a value type check condition matches (byte ptr [eax] == 2), converting a floating-point value on the x87 FPU stack into an integer via SSE (__ftol2_sse) when the value is handled.
- `VM_OpcodeHandler_Slot22` (0x0040ADF0) - call at 0x0040ADFF  
  Called to convert a floating-point operand value to an integer when the operand type indicator is 2. The resulting integer from st(0) is used as an array index or identifier in subsequent validation checks.
- `ScheduleTimedEventOrState` (0x0040AD00) - call at 0x0040AD3E  
  Called to convert a floating-point value loaded from the script argument structure into a 32-bit integer when the argument type indicates it should be handled as an integer.
- `ExecuteScriptMathOrConversionOp` (0x0040AC70) - call at 0x0040AC87  
  Called as part of an inline helper or conversion sequence to convert a floating-point value on the x87 stack into an integer when a specific type tag condition is met in the script operand structure.
- `ExecuteOpcode19` (0x0040AC10) - call at 0x0040AC1F  
  Called when an operand structure's type tag equals 2 (indicating a floating-point value), converting the floating-point number currently loaded on the FPU stack into an integer.
- `HandleBytecodeStatusOrEvent` (0x0040ABE0) - call at 0x0040ABEC  
  Called to convert a floating-point value (currently on the x87 FPU stack via fld) into an integer when the operand type indicator at [eax] equals 2.
- `ExecuteBytecodeVtableSlot17` (0x0040AB80) - call at 0x0040AB8F  
  Converts a floating-point value loaded onto the FPU stack into a 32-bit integer when the variant type byte at the start of the argument object equals 2.
- `ExecuteBytecodeTypeOp` (0x0040A880) - call at 0x0040A88F  
  Called to convert a floating-point number (loaded onto the FPU stack when the argument byte at `[eax]` equals 2) into an integer value via SSE, returning the integer result.
- `VM_OpcodeHandler_Slot11` (0x0040A830) - call at 0x0040A83E, 0x0040A858  
  Called twice (at 0x0040A83E and 0x0040A858) when a type flag byte in an argument structure equals 2, converting a floating-point value loaded on the FPU stack into a 32-bit integer returned in EAX.
- `ExecuteBytecodeOp_10` (0x0040A7E0) - call at 0x0040A7EE, 0x0040A808  
  Called twice within conditional branches (depending on operand type tags) to convert a floating-point value currently on the FPU top-of-stack (st(0)) into an integer, with the resulting integer value then stored into a register for subsequent arithmetic and state updates.
- `ExecuteSpawnEntityScriptOpcode` (0x0040A3F0) - call at 0x0040A407, 0x0040A41C  
  Called to convert floating-point values extracted from the operand descriptor structure into integer values when type flags dictate truncation.
- `OpcodeRandomRange` (0x0040A2D0) - call at 0x0040A2E3, 0x0040A2F7  
  Called to convert a floating-point operand value from the x87 floating-point stack into a 32-bit integer whenever an operand type tag indicates a floating-point representation (type value 2). It is invoked conditionally for either the first or second operand (or both) before comparing and computing values.
- `ProcessRtsResourceSpawningOrPathNodes` (0x00407B50) - call at 0x00407E4F, 0x00407E96  
  Called multiple times to convert floating-point values from the x87 FPU stack into integer indices (truncated via __ftol2_sse) for table lookup and discretization.
- `SpawnResourceOrEntityBatch` (0x00402050) - call at 0x004021F0, 0x0040224C, 0x004023B9, 0x0040240D  
  Invoked via the standard __ftol2_sse helper to convert floating-point values in st(0) into integers for grid/index clamping and boundary checks.
- `CalculateGridIndex` (0x00417410) - call at 0x00417427, 0x00417462  
  Called twice within the function to convert intermediate floating-point calculation results in the x87 FPU stack into integer values (via __ftol2_sse) so they can be clamped and combined into a final coordinate/index returned in eax.
- `ConvertAndClampCoordinate` (0x00414AA0) - call at 0x00414AB4  
  Converts a floating-point calculation result (after multiplying a float from the FPU stack by a constant scaling factor at 0x0041D960) into a signed 32-bit integer. The resulting integer is subsequently used for comparison, bounds-checking against 0 and 95 (0x5F), and as a clamped index or coordinate output.
- `UpdateResourceStateBlockOrSimulationStep` (0x00416640) - call at 0x00416845, 0x00416889  
  Called twice to convert floating-point values computed on the x87 FPU stack into integer indices (returned in `eax`), which are then bounds-checked and clamped.
- `UpdateParticleBatchState` (0x004156B0) - call at 0x004156F8  
  Called inside the inner loop (executing 96 times total per invocation) to convert a floating-point result into a signed 32-bit integer, whose value is then clamped, adjusted, and stored into state arrays.
- `UpdateParticleSystemOrEmitter` (0x00415D40) - call at 0x0041652D, 0x00416576  
  Called twice consecutively during particle generation/coordinate calculation loops to convert floating-point values from the top of the FPU stack into 32-bit integers via SSE, used for array indexing or bit-mask operations.
- `CalculateGridPointerOrMetric` (0x00414AF0) - call at 0x00414B63, 0x00414B74  
  Called twice in succession (at 0x00414B63 and 0x00414B74) to convert floating-point values from the x87 FPU stack into 32-bit integer coordinates or indices. The resulting integers are used in pointer arithmetic and index calculations combined with an input base pointer.
- `ComputeNoiseOrRandomCellValues` (0x00414FA0) - call at 0x00414FAD, 0x00414FD2  
  Converts a floating-point value currently on the x87 FPU stack into an integer (using standard SSE/FPU conversion semantics) to be used in subsequent integer arithmetic and bit-mixing operations.

### InitializeSubsystemRecords - `0x00401F60`

_original name `sub_401F60` | `void __stdcall InitializeSubsystemRecords(void *resourceBlock, unsigned int stateFlags)` | confidence: high, clear control flow patterns, consistent use of known callee sub_404150, and well-understood x86 math/loop structures. | source: llm_

**Initializes an array of structured configuration records within a large resource block using a loop, followed by setting up two additional specific subsystem records.**

The function checks a count field at offset +0x5BBBC in the provided resource block. If greater than zero, it enters a loop that iterates that many times. On each iteration, it performs floating-point arithmetic combining constants and bit-extracted flags from the second argument, stores temporary values locally, sets header fields (such as writing 0x1F4h at offset -4 and 0x96h at offset 0 relative to the record base), and calls sub_404150 [InitializeRecord]. After the loop completes, it performs two additional calls to sub_404150 [InitializeRecord] with specific fixed parameters and computed floating-point configurations.

**Parameters:**
- `resourceBlock` (void *, [esp+1Ch+arg_0]) - Pointer to the large resource/state block structure being initialized.
- `stateFlags` (unsigned int, [esp+1Ch+arg_4]) - State flags or configuration bitmask used inside the loop to compute per-record parameters.

**Returns:** none

**Side effects:**
- Modifies fields within the resourceBlock structure including array elements and configuration records at offsets +0x420 and +0x528.

**Data structures:**
- ResourceBlock: +0x5BBBC int recordCount, array of records starting after an offset, +0x420 secondary record, +0x528 tertiary record

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x004017ED  
  Called with the resource block pointer (ebx) and command-line argument/state pointer (arg_4) to perform an early processing or configuration phase.

**Calls:**
- `InitializeRecord` (0x00404150) - call at 0x00401FBD, 0x0040201B, 0x0040203A

### InitializeRecord - `0x00404150`

_original name `sub_404150` | `void __fastcall InitializeRecord(void *this_ptr, void *edx_unused, unsigned char arg_0, unsigned char arg_4, const void *arg_8)` | confidence: high | complete disassembly mapping and clear custom calling conventions verified via caller context | source: llm_

**Initializes a structured configuration record in a state buffer by setting field bytes, zeroing blocks with memset, computing a bit shift value, and copying pointers from an input structure.**

The function takes an object or buffer pointer in `esi` (using a custom fastcall convention along with `bl`, `dl`), copies input bytes `arg_0` and `arg_4` (and `dl`, `bl`) into specific header offsets of the structure, zeroes out multiple internal block regions using `memset`, resets several 32-bit fields to zero, computes a bitmask or shift value (`1 << bl`) stored at offset 0xE8, and copies pointers from a supplied source structure pointer (`arg_8`) into offsets 0xEC and 0xF0.

**Parameters:**
- `this_ptr` (void *, esi) - Pointer to the main structure/record being initialized.
- `param_bl` (unsigned char, bl) - Used as a byte value for the record and a shift count for a bitmask.
- `param_dl` (unsigned char, dl) - Used as a byte value written to offset +3.
- `arg_0` (unsigned char, esp+4) - Byte value written to offset +1.
- `arg_4` (unsigned char, esp+8) - Byte value written to offset +2.
- `arg_8` (const void *, esp+C) - Source structure/data pointer providing values for offsets 0xEC and 0xF0.

**Returns:** none

**Side effects:**
- Modifies the memory structure pointed to by esi, including calls to memset.

**Data structures:**
- Record structure: +0x0 byte bl, +0x1 byte arg_0, +0x2 byte arg_4, +0x3 byte dl, +0x4-0x13 zeroed fields, +0x14-0x75 memset buffer (0x62 bytes), +0x76-0xD7 memset buffer (0x62 bytes), +0xD8-+0xE4 zeroed dwords, +0xE8 bitshift value (1 << bl), +0xEC dword from [ebp+0], +0xF0 dword from [ebp+4], +0xF4-+0x104 zeroed dwords.

**Called by:**
- `InitializeSubsystemRecords` (0x00401F60) - call at 0x00401FBD, 0x0040201B, 0x0040203A  
  Called multiple times during initialization: once inside a loop over the resource block entries (using a counter-dependent configuration with floating-point math, pushing an address, a loop index, and a bitfield derived from arg_4), and twice afterward with specific constant parameters (e.g., pushing integers like 2, 4, 5 and local/data pointers while setting up configuration types in registers `dl` and `ebx`).

**Calls:**
- `memset` (0x0041A150) - call at 0x0040417E, 0x0040418A

### InitializeSubManager - `0x004186E0`

_original name `sub_4186E0` | `void __fastcall InitializeSubManager(void *manager)` | confidence: high, clean loop initialization over an allocated block with distinct stride and index patterns. | source: llm_

**Initializes a sub-manager structure by allocating an 8000-byte block and setting up a linked list or table of 4096 (0x1000) entries with zeroed values and index links.**

The function sets up a management structure passed in the `esi` register. It configures two fields at offsets `+4` (set to `0x1000`) and calls `AllocateCustomBlock` with a size of `0x8000` (32,768 bytes), storing the resulting pointer at offset `+0`. It then enters a loop running `0x1000` (4096) iterations. In each iteration, it populates an 8-byte element structure: setting the 4-byte payload field at `base + index*8` to zero, the 2-byte field at `base + index*8 + 4` to 1, and the 2-byte field at `base + index*8 + 6` to the next index (`index + 1`). Finally, it clears two dword fields at offsets `+8` and `+0x0C` in the main structure.

**Parameters:**
- `manager` (void*, esi) - Pointer to the sub-manager structure being initialized.

**Returns:** none

**Side effects:**
- Allocates a memory block via AllocateCustomBlock
- Initializes fields within the structure passed in esi

**Data structures:**
- Manager: +0x0 void* block_ptr, +0x4 int capacity (0x1000), +0x8 int field_8, +0x0C int field_C
- Entry: +0x0 int value_0, +0x4 short value_4, +0x6 short next_index

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401791  
  Called immediately after the first initialization routine, operating on another offset within the resource block (esi = ebx + 0x51E80), likely initializing a companion collection or manager.

**Calls:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004186EC

### AllocateCustomBlock - `0x004185D0`

_original name `sub_4185D0` | `void* __fastcall AllocateCustomBlock(int size)` | confidence: high | complete disassembly context and clear interaction with custom pool and malloc allocators | source: llm_

**Allocates a custom memory block of the requested size, utilizing a fixed-size memory pool allocator for small/medium allocations or falling back to malloc for large ones, and returns an inner payload pointer with a tracked header.**

The function checks if the global allocation subsystem has been initialized (`byte_42285C`), calling `sub_4184E0 [InitializeSubsystem]` if not. If the requested size in `eax` is 0, it defaults to 1 byte. It then queries a bin/pool index via `sub_418430 [FindAllocationBinIndex]`. If no matching pool bin is found (returned index < 0), it falls back to standard heap allocation via `malloc` requesting `size + 8` bytes, marks the allocation header with `0xFF`, and sets up an internal block header with signature 'RM' (`0x4D52`), the block size, and zeroes the payload via `memset`. If a pool bin index is found, it checks if a memory block chunk exists for that bin; if not, it calls `AllocateMemoryPoolBlock` to populate the pool. It then unlinks a free block from the bin's free list, stores the bin index (`bl`) at `[eax+2]`, updates global allocation statistics (`dword_42281C`), writes the 'RM' signature, size, and clears the user payload via `memset`. Finally, it returns a pointer to the user payload starting 8 bytes into the allocated block.

**Parameters:**
- `size` (int, eax) - Requested size of the memory block in bytes

**Returns:** Pointer to the allocated payload memory block (pointing to the user data area, offset +8 from the raw allocation block) in eax, or NULL on failure.

**Side effects:**
- Initializes global allocation subsystem structures on first use
- Allocates memory via malloc or internal pre-allocated pools
- Modifies global tracking data at 0x0042281C and 0x00422820
- Clears the user payload portion of the newly allocated block via memset

**Data structures:**
- Allocation Header: +0x0 WORD signature ('RM' / 0x4D52), +0x2 BYTE bin_index (0xFF for malloc'd blocks), +0x3 BYTE flags/padding, +0x4 DWORD size, +0x8 void* user_payload

**Called by:**
- `CreateSubtypeObject` (0x004145A0) - call at 0x004145CF, 0x004145F8, 0x00414621, 0x0041464A, 0x00414673, 0x00414698, 0x004146BD  
  Called with size 0x20 (32 bytes) in EAX to allocate raw memory for a new node or object instance across all switch cases. The returned pointer in EAX is checked for validity (non-zero) before proceeding with construction.
- `ResizeOrCompactContainer` (0x00403520) - call at 0x004035DD, 0x00403614  
  Called to allocate a new buffer block of size calculated in ebx when capacity needs to be grown or initialized, returning the newly allocated buffer pointer in eax.
- `ProcessPriorityQueueTick` (0x00419300) - call at 0x004193E8, 0x0041941E  
  Allocates a new memory buffer or data block of size specified in eax (computed from capacity/element size), called when growing or initializing storage for elements in the structure pointed to by ebx.
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401B42, 0x00401B78, 0x00401E31, 0x00401E67  
  Called at multiple points (via helper logic for dynamic buffer management / resizing) to allocate or reallocate memory blocks of size specified in `eax` (e.g., passing byte/element counts based on computed capacities).
- `CreateScriptVariable` (0x004027A0) - call at 0x004027AD  
  Called with size 24h (decimal 36) pushed as an argument before checking a global capacity or availability condition. Its non-zero return value determines whether a new object can be created or if allocation falls back to zero.
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x004025BA, 0x004025E7, 0x00402612, 0x0040263B  
  Allocates memory chunks of specific sizes (e.g., 0x2C8, 0x27C, 0x2B8, 0x24C) depending on the configuration flags retrieved from the lookup table, returning the newly created object pointer.
- `ReallocateCustomBlock` (0x00418670) - call at 0x004186A5; tail jump at 0x00418676  
  Called to allocate a new memory block of size edi when the existing buffer is either null (via tail jump) or lacks sufficient capacity. Returns the newly allocated buffer pointer in eax.
- `RtsComponentFactory_Create` (0x00411580) - call at 0x004115B5, 0x004115D1, 0x004115EE, 0x0041160B, 0x00411628, 0x00411645, 0x00411662, 0x0041167F (+8 more)  
  Called before instantiating any sub-component to allocate a block of memory (passing size 0x28, 0x20, or 0x1C in eax depending on the target case). The resulting pointer is checked for null before proceeding with constructor initialization.
- `InitializeSubManager` (0x004186E0) - call at 0x004186EC  
  Called as part of the sub-component or manager initialization sequence right after setting initial size/capacity configuration fields (setting `eax` to 0x8000 and `dword ptr [esi+4]` to 0x1000). The return value in `eax` (0x8000) is subsequently stored into `[esi]` to serve as a base pointer or handle for the table allocation initialized in the following loop.

**Calls:**
- `InitializeSubsystem` (0x004184E0) - call at 0x004185DE
- `FindAllocationBinIndex` (0x00418430) - call at 0x004185EE
- `AllocateMemoryPoolBlock` (0x00418470) - call at 0x00418622
- `memset` (0x0041A150) - call at 0x00418657

### AllocateMemoryPoolBlock - `0x00418470`

_original name `sub_418470` | `bool __fastcall AllocateMemoryPoolBlock(int bucket_index)` | confidence: high, clear heap allocation, linked list traversal, and global table indexing pattern. | source: llm_

**Allocates a large 64KB memory chunk via malloc, initializes a linked list of fixed-size sub-blocks within it for a specific pool bucket, and registers the new chunk with global tracking structures.**

The function requests 64KB (0x10000 bytes) of heap memory using `malloc`. If the allocation fails, it immediately returns false (0). On success, it links the new block into a global block list (`Block` at 0x00422858) and increments a global counter (`dword_422854`). It uses an index passed via `esi` (the bucket index) to look up a size configuration from a read-only table (`word_41B150`), computes how many sub-blocks fit into the 64KB chunk, and links them together by modifying pointers within the newly allocated chunk, prepending them to the free-list array stored at `dword_422820`. Finally, it returns true (1) in `al` to indicate success.

**Parameters:**
- `bucket_index` (int, esi) - Index of the memory pool bucket being expanded or populated.

**Returns:** bool in al (1 for success, 0 for allocation failure)

**Side effects:**
- Calls malloc to allocate 64KB of memory
- Modifies global tracking variables at 0x00422854 and 0x00422858
- Updates the free list head pointer for the specified bucket at 0x00422820[esi]

**Data structures:**
- MemoryChunk Header: +0x0 pointer to previous/next block, +0x4 bucket index

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401A28  
  Called inside a loop when allocating or retrieving blocks, acting as a capacity check or allocator helper to ensure underlying storage is ready; its boolean return value (`al`) is tested to abort on failure.
- `AllocateCustomBlock` (0x004185D0) - call at 0x00418622  
  Called if the slot/bin returned by `sub_418430` is empty or uninitialized (using the index/bucket stored in `ebx` passed via `esi`), to populate, expand, or allocate new blocks for that specific pool bucket. Returns a boolean success indicator in `al` which is checked to abort allocation if zero.

### FindAllocationBinIndex - `0x00418430`

_original name `sub_418430` | `int __fastcall FindAllocationBinIndex(int size)` | confidence: high | standard binary search pattern mapping a size against a static .rdata lookup table | source: llm_

**Performs a binary search over a predefined table of size thresholds (`word_41B150`) to find the appropriate pool or bin index for a given allocation size.**

The function implements a binary search algorithm. It takes a size value in `esi` (or via fastcall convention) and checks if it exceeds 0x800 (2048); if so, it returns -1 to indicate an out-of-range size. Otherwise, it searches an array of 16-bit words located at `0x0041B150` (`word_41B150`) using a standard lower-bound/binary search approach between indices `eax` (initialized to 0) and `edx` (initialized to 12). It compares the threshold found in the lookup table against the target size `esi`, adjusting the search range until the optimal bin index is isolated and returned in `eax`.

**Parameters:**
- `size` (int, esi) - The requested allocation size or metric to look up.

**Returns:** Returns the matching bin or pool index (0-12) in eax, or -1 (0xFFFFFFFF) if the size exceeds the maximum supported threshold (0x800).

**Data structures:**
- word_41B150: An array of 16-bit size thresholds in .rdata used by the memory pool allocator.

**Called by:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004185EE  
  Called with size/index information derived from the requested allocation size (`edi`) stored in `esi`, presumably to query or locate a suitable free-list slot, bin, or pool index from the internal allocator state.

### InitializeSubsystem - `0x004184E0`

_original name `sub_4184E0` | `void __cdecl InitializeSubsystem()` | confidence: high, straightforward initialization guard checking a flag and zeroing out a block of global state variables | source: llm_

**Initializes the custom allocation subsystem and its associated global tracking structures once upon the first allocation request.**

The function first checks a global initialization flag (byte_42285C). If the flag is non-zero, it immediately returns. If the flag is zero, it zeroes out a contiguous series of global dword variables and pointers spanning from 0x0042281C to 0x00422858, and then sets the global initialization flag (byte_42285C) to 1 to ensure this setup routine only executes once.

**Returns:** none

**Side effects:**
- modifies global initialization flag byte_42285C
- zeroes out multiple global variables and tracking pointers from 0x0042281C through 0x00422858

**Called by:**
- `AllocateCustomBlock` (0x004185D0) - call at 0x004185DE  
  Called once at the beginning of the function if a global initialization flag (`byte_42285C`) is not set, presumably to initialize the internal allocation sub-system or lookup tables before proceeding with the allocation request.

### InitializePrngState - `0x00418760`

_original name `sub_418760` | `void __fastcall InitializePrngState(unsigned int seed, void *state_struct)` | confidence: high | Recognized xorshift/PRNG initialization pattern with standard mixing constants. | source: llm_

**Initializes a state structure with a pseudo-random number generator (PRNG) state based on a seed, employing hash mixing constants and repeated mixing rounds.**

The function takes an input seed in `eax` and a destination structure pointer in `ecx`. It initializes a two-dword state buffer at `[ecx]` and `[ecx+4]` using XOR mixing with constants like `0x5DEECE66` and `0x2545F491`. If the second dword evaluates to zero, it defaults to the golden ratio/phi-derived constant `0x9E3779B9`. It then executes a series of mixing rounds combining multiplication (`imul eax` with an incrementing constant `0x3C6EF35F`) and a sequence of shift-XOR operations (specifically, xorshift mixing steps with shifts of 13, 11, and 5) to thoroughly diffuse the bits into the final state stored back into `[ecx]` and `[ecx+4]`.

**Parameters:**
- `seed` (unsigned int, eax) - Initial seed value used to derive the PRNG state.
- `state_struct` (unsigned int*, ecx) - Pointer to a 2-dword (8-byte) state buffer to be initialized.

**Returns:** none

**Side effects:**
- Writes 8 bytes of initialized PRNG state to the memory buffer pointed to by ecx.

**Data structures:**
- PRNG State Buffer: +0x00 unsigned int state0, +0x04 unsigned int state1

**Called by:**
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x00401786  
  Called early with a pointer derived from the resource block base (edi = ebx + 0x51EF8) to initialize a collection or subsystem structure.

### ResourceBlock_Initialize - `0x004012B0`

_original name `sub_4012B0` | `void* __cdecl ResourceBlock_Initialize(void *resourceBlock)` | confidence: high, clear constructor call with specific element counts and sizes, field initializations, and structured exception handling matching MSVC conventions. | source: llm_

**Initializes a large resource block data structure (approx 0x5BBE4 bytes) containing various internal lists, states, and an array of 7 elements via vector constructor.**

The function sets up a large resource block passed as a parameter (esi). It clears several fields (offsets +0x51E90 to +0x51E98) and initializes an integer flag at +0x51E9C to 1. It then invokes the vector constructor (__vec_ctor) at offset +0x51EA4 to construct an array of 7 elements, each 12 bytes in size, using ElementDestructor_12Bytes for cleanup and ResourceObject_Destructor for construction. It also zeroes out various other fields and status dwords throughout the block (ranging up to offset +0x573B8). An associated SEH/exception unwinding handler is set up to safely unwind the initialized vector elements in case of an exception. Finally, it returns the pointer to the initialized resource block in eax.

**Parameters:**
- `resourceBlock` (void *, [esp+4]) - Pointer to the allocated resource block (RM header region).

**Returns:** void* in eax, returning the pointer to the initialized resource block (esi).

**Side effects:**
- Initializes fields, counters, and vectors within the provided resource block memory region.
- Registers an SEH exception handler for vector construction cleanup.

**Data structures:**
- ResourceBlock: +0x51E90 int status/flags, +0x51EA4 array of 7 objects (12 bytes each)

**Called by:**
- `main` (0x00401050) - call at 0x004011F5  
  Called, passing the newly allocated memory buffer (at offset +8) as an argument, conditional on successful allocation; its return value is captured in esi and subsequently stored in a global variable (0x0042AC60).

**Calls:**
- `ResourceObject_Destructor` (0x00401690) - address taken at 0x004012FC
- `ElementDestructor_12Bytes` (0x00401680) - address taken at 0x00401301
- `__vec_ctor` (0x004199C2) - call at 0x00401315
- `__security_check_cookie` (0x004198F2) - call at 0x0041A369
- `__CxxFrameHandler3` (0x0041A224) - tail jump at 0x0041A373

### __CxxFrameHandler3 - `0x0041A224`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!__CxxFrameHandler3.**

A single `jmp ds:__imp___CxxFrameHandler3` through the import address table entry for MSVCR80!__CxxFrameHandler3 - calling this function is exactly calling __CxxFrameHandler3.

**Called by:**
- `main` (0x00401050) - tail jump at 0x0041A40E
- `ResourceObject_Cleanup` (0x00401490) - tail jump at 0x0041A343
- `CreateAndRegisterResourceEntity` (0x00402560) - tail jump at 0x0041A3E6
- `RtsObject_InitializeBase` (0x0040B100) - tail jump at 0x0041A39E
- `ResourceBlock_Initialize` (0x004012B0) - tail jump at 0x0041A373

### __vec_ctor - `0x004199C2`

_original name `??_L@YGXPAXIHP6EX0@Z1@Z` | `void __stdcall __vec_ctor(void *ptr, int element_size, int count, void (__cdecl *ctor)(void *), void (__cdecl *dtor)(void *))` | confidence: high | standard MSVC vector constructor helper implementation | source: llm_

**Constructs an array of objects using a provided constructor function, handling exception unwinding via `__ArrayUnwind` if any constructor throws an exception.**

The function sets up a standard MSVC SEH (Structured Exception Handling) frame. It then loops `count` times, advancing a loop counter and the current element pointer by `element_size` on each iteration, and invokes the constructor `ctor` (passed via `arg_C`) on each element via the `__thiscall` or standard calling convention (with `ecx` pointing to the object). If the loop completes successfully, a completion flag is set to 1. If an exception occurs during construction, the SEH exception filter/handler checks the completion flag and, if zero, invokes `__ArrayUnwind` to properly destruct the already-constructed elements in reverse order.

**Parameters:**
- `ptr` (void*, [ebp+arg_0]) - Pointer to the beginning of the array to construct.
- `element_size` (int, [ebp+arg_4]) - Size of each array element in bytes.
- `count` (int, [ebp+arg_8]) - Number of elements in the array.
- `ctor` (void (__cdecl *)(void*), [ebp+arg_C]) - Constructor function pointer to invoke for each element.
- `dtor` (void (__cdecl *)(void*), [ebp+arg_10]) - Destructor function pointer to pass to `__ArrayUnwind` if construction fails.

**Returns:** none

**Side effects:**
- Invokes the constructor function on each element of the array
- Triggers `__ArrayUnwind` and potential exception propagation if a constructor throws

**Called by:**
- `ResourceBlock_Initialize` (0x004012B0) - call at 0x00401315  
  Invoked as the MSVC vector constructor helper (`??_L@YGXPAXIHP6EX0@Z1@Z`) to initialize an array located at offset +0x51EA4 of the main resource structure. It is called with the array address, an element count of 7, an element size of 0Ch (12 bytes), the element destructor (`unknown_libname_1`), and the element constructor (`sub_00401690`).

**Calls:**
- `__SEH_prolog4` (0x00419DE8) - call at 0x004199C9
- `__SEH_epilog4` (0x00419E2D) - call at 0x00419A07
- `__ArrayUnwind` (0x00419901) - call at 0x00419A21

### __ArrayUnwind - `0x00419901`

_original name `?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z` | `void __stdcall __ArrayUnwind(void *ptr, int element_size, int count, void (__cdecl *dtor)(void *))` | confidence: high, standard MSVC runtime helper function for array unwinding with SEH | source: llm_

**Iterates backwards through an array of objects to invoke their destructor function, protected by a Structured Exception Handling (SEH) filter that calls terminate() if a C++ exception occurs during unwinding.**

The function sets up an SEH frame using __SEH_prolog4. It enters a loop that decrements the element count (`arg_8`). For each iteration, it adjusts the pointer (`arg_0`) backward by `element_size` (`arg_4`) and calls the destructor function pointer (`arg_C`) with the adjusted element pointer passed implicitly (or via the expected convention). If an exception is thrown during destruction, the SEH filter checks if the exception code is `0xE06D7363` (Microsoft C++ exception); if it is, it invokes `?terminate@@YAXXZ` to handle the nested exception scenario during unwinding. Otherwise, it propagates the exception. Upon normal completion of all elements, it tears down the SEH frame via __SEH_epilog4 and returns.

**Parameters:**
- `ptr` (void *, [esp+0x14]) - Base pointer to the array or the end-point depending on the caller's offset context.
- `element_size` (int, [esp+0x18]) - Size of each individual array element in bytes.
- `count` (int, [esp+0x1C]) - Number of elements to unwind.
- `dtor` (void (__cdecl *)(void *), [esp+0x20]) - Destructor function pointer to be invoked on each element.

**Returns:** none

**Side effects:**
- invokes element destructors on array items in reverse order
- calls std::terminate if a C++ exception occurs during array unwinding

**Called by:**
- `vector_destructor` (0x0041995F) - call at 0x004199BC  
  Called from the local cleanup helper function (at 0x004199AA) if an exception occurs mid-destruction before all elements are processed, passing the array pointer (`arg_0`), element count (`arg_8`), element size (`arg_4`), and the destructor function pointer (`arg_C`) to unwind the remaining elements.
- `__vec_ctor` (0x004199C2) - call at 0x00419A21  
  Called from an unwind cleanup helper (address 0x00419A21) when array construction is interrupted by an exception (indicated by var_20 being 0). It passes the base pointer (arg_0), the number of successfully constructed elements (var_1C), the element size (arg_4), and the element destructor function (arg_10) to unwind/destroy the partial array.

**Calls:**
- `__SEH_prolog4` (0x00419DE8) - call at 0x00419908
- `?terminate@@YAXXZ` (0x00419DE0) - call at 0x00419948
- `__SEH_epilog4` (0x00419E2D) - call at 0x00419957

### ?terminate@@YAXXZ - `0x00419DE0`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!?terminate@@YAXXZ.**

A single `jmp ds:__imp_?terminate@@YAXXZ` through the import address table entry for MSVCR80!?terminate@@YAXXZ - calling this function is exactly calling ?terminate@@YAXXZ.

**Called by:**
- `__ArrayUnwind` (0x00419901) - call at 0x00419948

### __SEH_epilog4 - `0x00419E2D`

_`void __cdecl __SEH_epilog4()` | confidence: high, standard compiler runtime helper with recognizable SEH frame tear-down sequence | source: llm_

**Tears down a standard MSVC Structured Exception Handling (SEH) frame, restoring the previous exception handler chain, stack pointer, and saved registers before returning.**

The function implements the standard MSVC compiler-generated SEH epilogue (`__SEH_epilog4`). It restores the previous SEH registration record by reading the saved pointer from `[ebp-10h]` and writing it back to the Thread Information Block (TIB) at `fs:0`. It then restores the preserved registers (`ebx`, `esi`, and `edi`), resets the stack pointer to `ebp`, pops the old base pointer, and performs an indirect return via a temporary return address preserved on the stack.

**Returns:** none

**Side effects:**
- restores the previous SEH exception handler in the TIB (fs:0)
- restores preserved registers ebx, esi, edi, ebp, esp

**Called by:**
- `vector_destructor` (0x0041995F) - call at 0x004199A2  
  Called near the end of the function (via `__SEH_epilog4`) to tear down the structured exception handling frame set up by the prologue.
- `__vec_ctor` (0x004199C2) - call at 0x00419A07  
  Called at the end of the function (address 0x00419A07) to tear down the standard MSVC Structured Exception Handling (SEH) frame before returning.
- `__ArrayUnwind` (0x00419901) - call at 0x00419957  
  Tears down the Structured Exception Handling (SEH) frame and restores the stack pointer just before the function returns successfully.

### __SEH_prolog4 - `0x00419DE8`

_`void __cdecl __SEH_prolog4(void)` | confidence: high, standard MSVC runtime library function (__SEH_prolog4) | source: llm_

**Sets up a standard Microsoft Visual C++ Structured Exception Handling (SEH) stack frame with security cookie checks and variable local frame allocation.**

The function begins by pushing the address of `__except_handler4` and chaining the new SEH record onto the thread's exception handler list via `fs:0`. It then adjusts `ebp` and allocates space on the stack based on a size argument passed by the caller. It incorporates security cookie protection by XORing the frame with `___security_cookie`, establishes the exception registration record fields, and saves the non-volatile registers `ebx`, `esi`, and `edi` before returning to the caller.

**Parameters:**
- `frame_size` (unsigned int, [esp+8]) - Size of the local stack frame to allocate

**Returns:** none

**Side effects:**
- updates the thread's exception handler chain at fs:0
- modifies esp and ebp
- pushes ebx, esi, and edi onto the stack

**Called by:**
- `vector_destructor` (0x0041995F) - call at 0x00419966  
  Called at the beginning of the function (via `__SEH_prolog4`) to set up standard structured exception handling (SEH) frame tracking for safe array destruction/unwinding.
- `__vec_ctor` (0x004199C2) - call at 0x004199C9  
  Called at the very beginning of the function (address 0x004199C9) to set up the standard MSVC Structured Exception Handling (SEH) frame (along with pushing 16h and the scope table) to protect the array construction loop.
- `__ArrayUnwind` (0x00419901) - call at 0x00419908  
  Initializes standard Structured Exception Handling (SEH) prolog at the start of the function, setting up the exception frame using local stack variables and a scope table descriptor.

**Calls:**
- `__except_handler4` (0x00419E41) - address taken at 0x00419DE8

### __except_handler4 - `0x00419E41`

_`int __cdecl __except_handler4(struct _EXCEPTION_RECORD *ExceptionRecord, void *EstablisherFrame, struct _CONTEXT *ContextRecord, void *DispatcherContext)` | confidence: high | standard Microsoft compiler runtime helper implementation matching known runtime disassembly | source: llm_

**Standard MSVC SEH exception handler shim that forwards exception records to _except_handler4_common along with the security cookie and checker function.**

This function acts as a wrapper for MSVC's Structured Exception Handling (SEH). It takes the four standard SEH callback parameters (ExceptionRecord, EstablisherFrame, ContextRecord, and DispatcherContext), pushes them onto the stack along with the global security cookie (`___security_cookie`) and the security cookie check function (`@__security_check_cookie@4`), and calls `_except_handler4_common`. After the common handler returns, it cleans up the 24 bytes (0x18) of arguments from the stack and returns the result.

**Parameters:**
- `ExceptionRecord` (struct _EXCEPTION_RECORD*, [esp+4]) - Pointer to an EXCEPTION_RECORD structure describing the exception.
- `EstablisherFrame` (void*, [esp+8]) - Pointer to the allocation or frame pointer of the establisher.
- `ContextRecord` (struct _CONTEXT*, [esp+0xC]) - Pointer to a CONTEXT structure containing the processor context at the time of the exception.
- `DispatcherContext` (void*, [esp+0x10]) - Pointer to the dispatcher context.

**Returns:** int (ExceptionDisposition or EXCEPTION_DISPOSITION, returned in eax from _except_handler4_common)

**Side effects:**
- invokes _except_handler4_common to process SEH unwinding and filtering
- validates or propagates security cookie checks during exception handling

**Called by:**
- `__SEH_prolog4` (0x00419DE8) - address taken at 0x00419DE8  
  Pushed onto the stack at the very beginning of the prolog (address 0x00419DE8) to be registered as the SEH exception filter/handler callback (`__except_handler4`) for the current activation record, managing unwinding when exceptions occur during operations like array construction or destruction.

**Calls:**
- `__security_check_cookie` (0x004198F2) - address taken at 0x00419E51
- `_except_handler4_common` (0x0041A120) - call at 0x00419E5B

### _except_handler4_common - `0x0041A120`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!_except_handler4_common.**

A single `jmp ds:__imp__except_handler4_common` through the import address table entry for MSVCR80!_except_handler4_common - calling this function is exactly calling _except_handler4_common.

**Called by:**
- `__except_handler4` (0x00419E41) - call at 0x00419E5B

### __security_check_cookie - `0x004198F2`

_original name `@__security_check_cookie@4` | `void __fastcall __security_check_cookie(uintptr_t stack_cookie)` | confidence: high | standard MSVC implementation of __security_check_cookie with a fastcall convention | source: llm_

**Validates that the stack canary value passed in ecx matches the global security cookie, jumping to the GS failure handler if a mismatch is detected.**

The function compares the 32-bit value in ecx against the global security cookie located at 0x0041F000. If the values are equal, the function immediately returns, allowing normal execution to proceed. If the values differ (indicating a stack buffer overflow or corruption), it performs a tail call to ___report_gsfailure to terminate the process.

**Parameters:**
- `stack_cookie` (uintptr_t, ecx) - The local stack cookie value retrieved from the stack frame before function epilog.

**Returns:** none

**Side effects:**
- terminates the process via ___report_gsfailure if stack corruption is detected

**Called by:**
- `main` (0x00401050) - call at 0x0041A404  
  Called as @__security_check_cookie@4 in the exception handling/SEH epilog to validate the stack canary before returning.
- `ResourceObject_Cleanup` (0x00401490) - call at 0x0041A339  
  Standard compiler-generated security cookie check (`@__security_check_cookie@4`) invoked during the function's exception handling / epilogue sequence to verify stack integrity.
- `WriteBufferHeaderOrPacket` (0x004090C0) - call at 0x00409157  
  Called with the stack canary value to perform standard security cookie verification before function exit.
- `WriteBufferPacket` (0x00408FE0) - call at 0x004090AE  
  Standard compiler-inserted security cookie validation check called right before function epilogue to ensure the stack hasn't been corrupted.
- `FlushAndCompressBuffer` (0x00409170) - call at 0x0040926B  
  Standard MSVC security cookie check called prior to function exit to ensure stack integrity.
- `CreateAndRegisterResourceEntity` (0x00402560) - call at 0x0041A3DC  
  Performs stack security cookie validation upon function exit to detect buffer overflows.
- `RtsObject_InitializeBase` (0x0040B100) - call at 0x0041A394  
  Invoked as part of the standard MSVC SEH/exception handler prologue cleanup via the frame handler (`__CxxFrameHandler3` / `SEH_40B100`) to validate the security cookie (`___security_cookie` XOR'd with `esp`) upon function exit.
- `ResourceBlock_Initialize` (0x004012B0) - call at 0x0041A369  
  Called from the SEH exception handler epilog (`SEH_4012B0`) to perform standard stack security cookie validation before unwinding or exiting.
- `__except_handler4` (0x00419E41) - address taken at 0x00419E51  
  The address of the security check function @__security_check_cookie@4 is passed as a callback or reference pointer to _except_handler4_common (along with the security cookie global variable at 0x0041F000 and the standard four SEH arguments) to ensure stack integrity is validated during exception handling and stack unwinding.

**Calls:**
- `___report_gsfailure` (0x00419CDB) - tail jump at 0x004198FC

### ___report_gsfailure - `0x00419CDB`

_`void __cdecl ___report_gsfailure(uintptr_t stack_cookie)` | confidence: high, standard MSVC runtime library function for buffer security failure reporting | source: llm_

**Standard MSVC buffer security check (GS) failure handler that saves processor and stack context, notifies the CRT debugger, invokes unhandled exception filtering, and forcefully terminates the process with STATUS_STACK_BUFFER_OVERRUN.**

When a stack cookie mismatch is detected, this function captures the current execution context by saving general-purpose registers (EAX, ECX, EDX, EBX, ESI, EDI), segment registers (SS, CS, DS, ES, FS, GS), and EFLAGS into a global storage area. It records the faulting return address and frame pointer, sets up an exception record with the status code 0xC0000409 (STATUS_STACK_BUFFER_OVERRUN), checks for an active debugger via IsDebuggerPresent, and invokes the CRT debugger hook (_crt_debugger_hook). It then unregisters any custom unhandled exception filters by calling SetUnhandledExceptionFilter(NULL) and passes the exception info structure to UnhandledExceptionFilter. If a debugger is not present, it calls the CRT debugger hook a second time before obtaining the current process handle via GetCurrentProcess and terminating the process via TerminateProcess with exit code 0xC0000409.

**Parameters:**
- `stack_cookie` (uintptr_t, [ebp+8] (pushed by caller or passed via register depending on compiler version, though here accessed via frame)) - The corrupted or mismatched stack security cookie value.

**Returns:** none

**Side effects:**
- saves full CPU register and state context to global variables
- calls CRT debugger hooks and Windows exception handling APIs
- terminates the current process immediately with status 0xC0000409 (STATUS_STACK_BUFFER_OVERRUN)

**Called by:**
- `__security_check_cookie` (0x004198F2) - tail jump at 0x004198FC  
  Invoked via a tail-call (`jmp`) when a stack security cookie mismatch is detected (`jnz loc_4198FC`), serving as the terminal handler for buffer overflow / stack corruption detection (GS failure reporting).

**Calls:**
- `_crt_debugger_hook` (0x0041A11A) - call at 0x00419DA1, 0x00419DC5

### _crt_debugger_hook - `0x0041A11A`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!_crt_debugger_hook.**

A single `jmp ds:__imp__crt_debugger_hook` through the import address table entry for MSVCR80!_crt_debugger_hook - calling this function is exactly calling _crt_debugger_hook.

**Called by:**
- `___report_gsfailure` (0x00419CDB) - call at 0x00419DA1, 0x00419DC5

### ElementDestructor_12Bytes - `0x00401680`

_original name `unknown_libname_1` | `void __thiscall ElementDestructor_12Bytes(void *this)` | confidence: high, the disassembly is extremely straightforward and directly matches a zeroing destructor for a 12-byte structure. | source: llm_

**Destroys or clears a 12-byte object/array element by zeroing out its first 12 bytes.**

The function takes a pointer to an object in the ecx register (characteristic of a __thiscall convention or a custom register-based helper). It clears the first 12 bytes (offsets +0x0, +0x4, and +0x8) of the target structure by writing zero to each 4-byte dword. It acts as a trivial element destructor callback passed to a vector constructor helper.

**Parameters:**
- `this` (void *, ecx) - Pointer to the 12-byte object/element to be cleared.

**Returns:** none

**Side effects:**
- Zeroes out the first 12 bytes of the structure pointed to by ecx.

**Data structures:**
- Object: +0x0 dword (field 1), +0x4 dword (field 2), +0x8 dword (field 3)

**Called by:**
- `ResourceBlock_Initialize` (0x004012B0) - address taken at 0x00401301  
  Passed as the destructor function pointer callback (element destructor) to the vector/array constructor helper `??_L@YGXPAXIHP6EX0@Z1@Z` for cleanup handling during array construction.

### ResourceObject_Destructor - `0x00401690`

_original name `sub_401690` | `void __thiscall ResourceObject_Destructor(void *this)` | confidence: high, clean structure, explicit validation of the 'RM' magic word, and standard memory free-list logic matched by caller context. | source: llm_

**Destroys or unlinks an individual RM-header resource object, updating global tracking counters and either freeing its memory or returning it to a custom free-list pool.**

The function expects a pointer via `ecx` pointing to a field containing a pointer to an internal resource block. If the pointer is null, it immediately returns. It navigates backwards by 8 bytes from the resource pointer to check a custom header, validating that the 16-bit word at offset +0 contains the magic value 'RM' (0x4D52). If the magic bytes do not match, it aborts cleanup and returns. Otherwise, it reads a size or count value at offset +4 and subtracts it from a global tracking variable (`dword_42281C`). It then checks a byte flag at offset +2; if this flag is 0xFF, it calls `free()` on the memory block (located at the header start). If the flag is not 0xFF, it treats the byte as an index, prepending the block onto a custom free-list array stored at `dword_422820` indexed by that byte.

**Parameters:**
- `this` (void*, ecx) - Pointer to an object containing a pointer to the resource block.

**Returns:** none

**Side effects:**
- frees memory via free() if the sub-pool flag is 0xFF
- modifies global tracking variable dword_42281C
- modifies custom free-list head pointers stored in dword_422820

**Data structures:**
- RM Header Block: -0x8 from inner pointer, +0x0 word signature (0x4D52 'RM'), +0x2 byte sub-pool index/flag, +0x4 dword size/count value, +0x8 start of data payload

**Called by:**
- `ResourceObject_Cleanup` (0x00401490) - address taken at 0x004015F2  
  The function address is pushed as a destructor callback (element destructor) to be used by the vector/array deletion helper `??_M@YGXPAXIHP6EX0@Z@Z` when destroying an array of 7 objects of size 12 bytes located at `edi + 0x51EA4`.
- `ResourceBlock_Initialize` (0x004012B0) - address taken at 0x004012FC  
  Passed as the constructor function pointer callback (element constructor) to the vector/array constructor helper `??_L@YGXPAXIHP6EX0@Z1@Z` to initialize elements within the newly allocated structure.

### memset - `0x0041A150`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!memset.**

A single `jmp ds:__imp_memset` through the import address table entry for MSVCR80!memset - calling this function is exactly calling memset.

**Called by:**
- `main` (0x00401050) - call at 0x004011E0
- `ResourceBuffer_FlushOrWritePacket` (0x00408B60) - call at 0x00408BF6
- `InitializeSimulationAndResourceSubsystem` (0x00401750) - call at 0x004017E0, 0x00401865, 0x00401895, 0x0040192C
- `InitializeRecord` (0x00404150) - call at 0x0040417E, 0x0040418A
- `AllocateCustomBlock` (0x004185D0) - call at 0x00418657

### InitCrc32Table - `0x004189C0`

_original name `sub_4189C0` | `void __cdecl InitCrc32Table(void)` | confidence: high, standard unrolled implementation of IEEE 802.3 CRC32 table initialization using polynomial 0xEDB88320 | source: llm_

**Initializes the standard CRC32 lookup table using the polynomial 0xEDB88320.**

The function loops 256 times from `ecx = 0` to `255`. In each iteration, it performs 8 consecutive bit-shift and conditional XOR operations corresponding to an unrolled computation for a single byte value (`ecx`). The resulting 32-bit CRC value is stored into the lookup table array `dword_422860` at index `ecx`. After the loop completes, it sets a global initialization flag (`byte_42285D`) to 1 to prevent re-initialization.

**Returns:** none

**Side effects:**
- populates the CRC32 lookup table at dword_422860 (256 dwords)
- sets the global initialization flag byte_42285D to 1

**Called by:**
- `main` (0x00401050) - call at 0x00401144  
  Called unconditionally right after sub_417ED0 during the initialization phase.
- `CalculateCrc32` (0x00418A60) - call at 0x00418A69  
  Called once at the beginning of the function if the global initialization flag at byte_42285D is zero, to initialize the CRC32 lookup table (dword_422860) before the checksum loop runs.

### InitSineTable - `0x00417ED0`

_original name `sub_417ED0` | `void __cdecl InitSineTable(void)` | confidence: high | the loop runs 1024 times, computes trigonometric values using _CIsin, and stores them sequentially into a data array. | source: llm_

**Initializes a precomputed sine or trigonometric lookup table containing 1024 float values.**

The function executes a loop from 0 to 1024 (0x400). In each iteration, it converts the loop index into a floating-point value, multiplies it by specific constants stored in the .rdata section, computes the sine using _CIsin, and stores the resulting floating-point value into an array or lookup table located in the .data section at 0x00421814.

**Returns:** none

**Side effects:**
- Populates a global floating-point lookup table in the .data section (starting at 0x00421814) with 1024 calculated values.

**Called by:**
- `main` (0x00401050) - call at 0x0040113F  
  Called unconditionally during early initialization after setting up global state, presumably to perform subsystem or engine startup.

**Calls:**
- `_CIsin` (0x0041A20C) - call at 0x00417EF6

### _CIsin - `0x0041A20C`

_confidence: high - no LLM needed | source: trivial: import thunk_

**Import thunk: jumps straight to the imported API MSVCR80!_CIsin.**

A single `jmp ds:__imp__CIsin` through the import address table entry for MSVCR80!_CIsin - calling this function is exactly calling _CIsin.

**Called by:**
- `InitSineTable` (0x00417ED0) - call at 0x00417EF6

## Warnings

- functions/0x00406760/callsites.response.txt: no callsite summary for 1 of 8 callees (0x00417F80) - falling back to plain call-site facts for those
- functions/0x0040BCD0/summary.response.txt: response isn't the expected JSON - using its raw text as the summary
- functions/0x00419590/summary.response.txt: response isn't the expected JSON - using its raw text as the summary
