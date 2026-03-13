import { useEffect, useState } from "react";
import { apiGet, apiPost } from "../services/api";

export default function Dashboard() {
  const [status, setStatus] = useState("loading");
  const [chain, setChain] = useState([]);
  const [message, setMessage] = useState("");
  const [resetting, setResetting] = useState(false);

  async function loadDashboard() {
    setStatus("loading");

    try {
      const [health, chainResponse] = await Promise.all([
        apiGet("/health"),
        apiGet("/chain"),
      ]);

      setStatus(health.ok && health.data.status === "ok" ? "online" : "offline");
      setChain(Array.isArray(chainResponse.data) ? chainResponse.data : []);
    } catch {
      setStatus("offline");
      setChain([]);
    }
  }

  async function handleReset() {
    const confirmed = window.confirm(
      "Reset the blockchain to the genesis block and clear registered users?"
    );

    if (!confirmed) {
      return;
    }

    setResetting(true);
    setMessage("");

    try {
      const { ok, data } = await apiPost("/reset", {});
      if (ok && data.success) {
        setMessage("Blockchain reset on the server. Browser keys were kept.");
        await loadDashboard();
      } else {
        setMessage(data.message || "Reset failed.");
      }
    } catch {
      setMessage("Reset request failed. Check whether the server is running.");
    } finally {
      setResetting(false);
    }
  }

  useEffect(() => {
    loadDashboard();
  }, []);

  return (
    <div className="max-w-3xl mx-auto mt-10 bg-white p-6 rounded shadow">
      <div className="flex items-start justify-between gap-4 mb-6">
        <h1 className="text-3xl font-bold">System Dashboard</h1>
        <button
          type="button"
          onClick={handleReset}
          disabled={resetting}
          className="bg-red-600 text-white px-4 py-2 rounded disabled:opacity-60"
        >
          {resetting ? "Resetting..." : "Reset Blockchain"}
        </button>
      </div>

      <p className="text-lg mb-2">
        Server Status:
        {status === "loading" && (
          <span className="ml-2 text-yellow-500 font-semibold">Checking</span>
        )}
        {status === "online" && (
          <span className="ml-2 text-green-600 font-semibold">Online</span>
        )}
        {status === "offline" && (
          <span className="ml-2 text-red-600 font-semibold">Offline</span>
        )}
      </p>

      <p className="text-lg mb-2">
        Total Blocks: <strong>{chain.length}</strong>
      </p>

      {message && <p className="mt-4 text-sm text-gray-700">{message}</p>}
    </div>
  );
}
